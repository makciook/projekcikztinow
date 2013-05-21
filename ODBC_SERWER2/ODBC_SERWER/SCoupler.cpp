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
	int size = 0;

	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
		if (nError == 0)											// timeout
		{
			cout << "nError = 0";
			break;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			cout << "Socket error";
			break;
		}
		unsigned int size;
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			cout << "dataLength\n";
			break;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			break;
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
			break;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			break;
		}
		buffer = new char[size+1];
		dataLength = recv(sock, buffer, size, 0);
		buffer[size] = '\0';
		if (dataLength == 0)										// client disconnected
		{
			cout << "dataLength\n";
			break;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "winsockerror\n";
			break;
		}
		buf = buffer;
		string suma = md5(buf);
		cout << "CheckSum: " << suma << "\n";
		if(strcmp(suma.c_str(), checksum) == 0)
		{
			std::cout << "Ok\n";
			send(sock, "Ok\0", 3, 0); 
		}
		else
		{
			std::cout << "Error\n";
			send(sock, "Error\0", 6, 0);
		}
		cout << "Msg: " << buf << "\n";
		parent->decrypt(buffer,size);
		
		resend = false;
	}
	sendMessage("Hello",sizeof("Hello"));
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
	int counter = 0;
	while(resend)
	{
		/*std::cout << "Wysylam: (len = " << bufSize;
		for(int i = 0; i < bufSize; ++i)
			std::cout << buffer[i];
		std::cout << "\n";*/
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
	//int ret = 0;
	//delete [] buffer;
	//return ret;
	return 0;
}