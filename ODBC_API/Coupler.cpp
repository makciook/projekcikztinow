#include "Coupler.h"
#include "Md5.h"

Coupler::Coupler(Crypter *parent)
{
	this->parent = parent;
	int timeout = 500;
    tv.tv_sec = timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;
}


Coupler::~Coupler(void)
{

}


int Coupler::sendAndWait(const string &msg)
{
	string checkSum;
	unsigned int size = msg.length();
	checkSum = md5(msg);
	char s[10];
	_itoa_s(size, s, 10);
	string buffer(s);
	buffer += checkSum;
	buffer += msg;
	bool resend = true;
	char response[10];
	while(resend)
	{
		if(send(sock, buffer.c_str(), buffer.length(), 0) == SOCKET_ERROR)
			return 1;
		int dataLength = recv(sock, response, sizeof(response), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		int nError = WSAGetLastError(); 
		if (nError != WSAEWOULDBLOCK)								// winsock error
		{
			return 4;
		}
		if(strcmp(response, "Ok") == 0)
			resend = false;
	}
	waitForMessage();
}

int Coupler::sendAndLeave(void)
{
	
}


int Coupler::waitForMessage(void)
{
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	bool resend = false;
	string buf;

	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);
		if (nError == 0)											// timeout
		{
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			return 2;
		}
		unsigned int size;
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		int nError = WSAGetLastError(); 
		if (nError != WSAEWOULDBLOCK)								// winsock error
		{
			return 4;
		}
		size = ntohl(size);
		char checksum[128];
		dataLength = recv(sock, checksum, 128, 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != WSAEWOULDBLOCK)								// winsock error
		{
			return 4;
		}
		char *buffer = new char[size];
		dataLength = recv(sock, buffer, size, 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != WSAEWOULDBLOCK)								// winsock error
		{
			return 4;
		}
		buf = buffer;
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
	parent->decrypt(buf);
}


int Coupler::conn(string addr)
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long opt = 1;
	//ioctlsocket(sock, FIONBIO, &opt);								// set socket to nonblocking

	if(sock == INVALID_SOCKET)
		return 1;
	SockAddr.sin_port = htons(27015);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
	if(connect(sock, (SOCKADDR *)(&SockAddr), sizeof(SockAddr)) == SOCKET_ERROR) 
		return 2;
	char klucz[32];
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);

    int nError = select(0, &fd, NULL, NULL, &tv);
    if (nError == 0)											// timeout
    {
		return 3;
    }

    if (nError == SOCKET_ERROR)									// winsock error
    {
		return 4;
    }
	int dataLength = recv(sock, klucz, sizeof(klucz), 0);		// odebranie klucza
	if (dataLength == 0)										// client disconnected
    {
		return 5;
    }
	if (nError != WSAEWOULDBLOCK)								// winsock error
    {
		return 6;
    }
	parent->setKey(klucz);
	return 0;
}


void Coupler::disconn(void)
{
	closesocket(sock);
}