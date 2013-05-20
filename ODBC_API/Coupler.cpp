#include "Coupler.h"
#include "Md5.h"
#include "Crypter.h"

Coupler::Coupler(Crypter *parent)
{
	this->parent = parent;
	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;
}


Coupler::~Coupler(void)
{

}


int Coupler::sendAndWait(const char* msg, int length)
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
	int ret = waitForMessage();
	delete [] buffer;
	return ret;
}

int Coupler::sendAndLeave(void)
{
	return 0;
}


int Coupler::waitForMessage(void)
{
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	bool resend = true;
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
		char checksum[33];
		dataLength = recv(sock, checksum, 32, 0);
		checksum[32] = '\0';
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		buffer = new char[size+1];
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
		string buf(buffer);
		string checkSum(checksum);
		if(md5(buf) != checkSum)
		{
			send(sock, "Error\0", 6, 0); 
			resend = true;
		}
		else
		{
			send(sock, "Ok\0", 3, 0); 
			resend = false;
		}
	}
	int ret = parent->decrypt(buffer, size);
	delete [] buffer;
	return ret;
}


int Coupler::conn(const char* addr)
{
	WSADATA WsaDat;
    
	if(WSAStartup(MAKEWORD(2,2),&WsaDat) != 0)
		return 1;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sock == INVALID_SOCKET)
		return 1;
	SockAddr.sin_port = htons( 27017 );
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.S_un.S_addr = inet_addr(addr);
	std::cout << "Adres: \"" << addr << "\"\n";
	if(connect(sock, (SOCKADDR *)(&SockAddr), sizeof(SockAddr)) == SOCKET_ERROR) 
		return 2;
	unsigned long opt = 1;
	ioctlsocket(sock, FIONBIO, &opt);								// set socket to nonblocking
	char klucz[33];
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	std::cout << "Polaczono. Czekam na dane ...\n";
    int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
    if (nError == 0)											// timeout
    {
		return 3;
    }

    if (nError == SOCKET_ERROR)									// winsock error
    {
		return 4;
    }
	int dataLength = recv(sock, klucz, sizeof(klucz), 0);		// odebranie klucza
	klucz[32] = '\0';
	if (dataLength == 0)										// client disconnected
    {
		return 5;
    }
	nError = WSAGetLastError();
	if (nError != 0)											// winsock error
    {
		return 6;
    }
	parent->setKey(klucz);
	std::cout << "Ustawinono klucz " << klucz << "\n";
	return 0;
}


void Coupler::disconn(void)
{
	closesocket(sock);
}