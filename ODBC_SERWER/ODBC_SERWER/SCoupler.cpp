#include "SCoupler.h"


SCoupler::SCoupler()
{
	parent = new Crypter(this);
	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;
}

SCoupler::~SCoupler(void)
{
}

int SCoupler::waitForMessage()
{
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	bool resend = true;
	string buf;
	char *buffer;
	unsigned int size;

	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
		if (nError == 0)											// timeout
		{
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			return 2;
		}
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		size = ntohl(size);
		char checksum[32];
		dataLength = recv(sock, checksum, sizeof(checksum), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		buffer = new char[size];
		dataLength = recv(sock, buffer, size, 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		buffer[size] = '\0';
		buf = buffer;
		string checkSum(checksum);
		checkSum = checkSum.substr(0,32);
		if(md5(buf) != checkSum)
		{
			send(sock, "Error\0", 6, 0); 
			resend = true;
		}
		else
		{
			send(sock, "Ok\0", 3, 0); 
			std::cout<<"OK"<<std::endl;
			resend = false;
		}
	}
	cout<<"Message: "<<buf.c_str()<<endl;
	int ret = parent->decrypt(buf.c_str(), size);
	//delete [] buffer;
	return ret;
	return 0;
}

int SCoupler::sendMessage(const char* msg, int length)
{
	string checkSum;
	int size = length;
	checkSum = md5(msg);
	int bufSize = size+checkSum.length()+sizeof(size)+1;
	char *buffer = new char[bufSize];
	size = htonl(size);
	memcpy(buffer, &size, sizeof(size));
	memcpy(buffer+sizeof(size), checkSum.c_str(), checkSum.length());
	memcpy(buffer+sizeof(size)+checkSum.length(), msg, length);
	buffer[bufSize-1] = '\0';
	bool resend = true;
	char response[10];
	while(resend)
	{
		if(send(sock, buffer, bufSize, 0) == SOCKET_ERROR)
			return 1;
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
		std::cout << "Czekam na odpowiedz\n";
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
		if (nError == 0)											// timeout
		{
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			return 2;
		}
		int dataLength = recv(sock, response, sizeof(response), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		std::cout << "Odebrano odpowiedz: " << response << "\n";
		if(strcmp(response, "Ok") == 0)
			resend = false;
	}
	int ret = waitForMessage();
	//delete [] buffer;
	return ret;
	return 0;
}