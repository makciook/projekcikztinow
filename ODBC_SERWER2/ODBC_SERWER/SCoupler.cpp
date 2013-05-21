#include "SCoupler.h"
#include "Crypter.h"


SCoupler::SCoupler()
{
	parent = new Crypter(this);

	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;

	parent->setKey("01234567890123456789012345678901");
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
	char *buffer = NULL;
	unsigned int size = 0;

	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
		if (nError == 0)											// timeout
		{
			cout << "nError = 0";
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			cout << "Socket error";
			return 1;
		}
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			cout << "dataLength\n";
			return 1;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			return 1;
		}
		size = ntohl(size);
		cout << "Size: " << size << "\n";
		char checksum[33];
		dataLength = recv(sock, checksum, 32, 0);
		checksum[32] = '\0';
		cout << "MD5: " << checksum << "\n";
		if (dataLength == 0)										// client disconnected
		{
			cout << "dataLength\n";
			return 1;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			return 1;
		}
		buffer = new char[size+1];
		dataLength = recv(sock, buffer, size, 0);
		buffer[size] = '\0';
		if (dataLength == 0)										// client disconnected
		{
			cout << "dataLength\n";
			return 1;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			return 1;
		}
		buf = buffer;
		string suma = md5(buf);
		cout << "CheckSum: " << suma << "\n";
		if(strcmp(suma.c_str(), checksum) == 0)
		{
			std::cout << "Ok\n";
			send(sock, "Ok\0", 3, 0); 
			resend = false;
		}
		else
		{
			std::cout << "Error\n";
			send(sock, "Error\0", 6, 0);
			resend = true;
		}
		cout << "Msg: " << buf << "\n";
	
	}
	cout << "Buffer: " << buf << "\n";
	parent->decrypt(buf.c_str(),size);
	
}

int SCoupler::sendMessage(const char* msg, int length)
{
	string checkSum;
	int size = length;
	checkSum = md5(msg);
	int bufSize = size+checkSum.length()+sizeof(size)+1;
	char *buffer = new char[bufSize];
	std::cout << "Wysylam size: " << size << "\n";
	size = htonl(size);
	memcpy(buffer, &size, sizeof(size));
	std::cout << "Wysylam suma: " << checkSum << "\n";
	std::cout << "Wysylam buf: " << string(msg) << "\n";
	memcpy(buffer+sizeof(size), checkSum.c_str(), checkSum.length());
	memcpy(buffer+sizeof(size)+checkSum.length(), msg, length);
	buffer[bufSize-1] = '\0';
	bool resend = true;
	char response[10];
	int counter = 0;
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(sock, &fd);
	while(resend)
	{
		/*std::cout << "Wysylam: (len = " << bufSize;
		for(int i = 0; i < bufSize; ++i)
			std::cout << buffer[i];
		std::cout << "\n";*/
		if(send(sock, buffer, bufSize, 0) == SOCKET_ERROR)
			return 1;
		
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
		std::cout << "Odebrano odpowiedz: ";
		for(int i = 0; i < sizeof(response); ++i)
			std::cout << response[i];
		std::cout << "\n";
		if(strcmp(response, "Ok") == 0)
			resend = false;
		else
		{
			++counter;
			if(counter > 2)											// maksymalnie 3 razy ponawiamy próbê wys³ania
			{
				delete [] buffer;
				return 5;
			}
		}
	}
	if(buffer != NULL)
		delete [] buffer;
	return 0;
}