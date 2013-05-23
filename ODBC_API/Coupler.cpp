#include "Coupler.h"
#include "Md5.h"
#include "Crypter.h"

using namespace std;

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
	checkSum = md5(string(msg, length));
	int bufSize = size+checkSum.length()+sizeof(size);
	char *buffer = new char[bufSize];
	cout << "Wysylam size: " << size << "\n";
	cout << "wysylam suma: \"" << checkSum << "\"\n";
	cout << "Wysylam msg: \"";
	for(int i = 0; i < length; ++i)
		cout << msg[i];
	cout << "\"\n";
	size = htonl(size);
	memcpy(buffer, &size, sizeof(size));
	memcpy(buffer+sizeof(size), checkSum.c_str(), checkSum.length());
	memcpy(buffer+sizeof(size)+checkSum.length(), msg, length);
	bool resend = true;
	char response[10];
	int counter = 0;
	fd_set fd;
	while(resend)
	{
		/*std::cout << "Wysylam: (len = " << bufSize;
		for(int i = 0; i < bufSize; ++i)
			std::cout << buffer[i];
		std::cout << "\n";*/
		if(send(sock, buffer, bufSize, 0) == SOCKET_ERROR)
			return 1;
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
		std::cout << "Odebrano odpowiedz: \"";
		for(int i = 0; i < 10; ++i)
			cout << response[i];
		cout << "\"\n";
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
	int counter = 0;
	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
		if (nError == 0)											// timeout
		{
			cout << "Timeout\n";
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			cout << "Error: " << nError << "\n";
			return 2;
		}
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError: " << nError << "\n";
			return 4;
		}
		size = ntohl(size);
		cout << "Odebrano size: " << size << "\n";
		char checksum[33];
		dataLength = recv(sock, checksum, 32, 0);
		checksum[32] = '\0';
		cout << "Odebrano suma: \"" << checksum << "\"\n";
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength 2\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError 3: " << nError << "\n";
			return 4;
		}
		buffer = new char[size+1];
		dataLength = recv(sock, buffer, size, 0);
		cout << "Odebrano buf: \"";
		for(int i = 0; i < size; ++i)
			cout << buffer[i];
		cout << "\"\n";
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength 3\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError 4: " << nError << "\n";
			return 4;
		}
		buffer[size] = '\0';
		string buf(buffer, size);
		string checkSum(checksum);
		if(md5(buf) != checkSum)
		{
			++counter;
			if(counter > 2)
			{
				send(sock, "Stop\0", 5, 0);
				cout << "Stopped send!\n";
				return 5;
			}
			send(sock, "Error\0", 6, 0); 
			cout << "Error send\n";
			resend = true;
		}
		else
		{
			send(sock, "Ok\0", 3, 0); 
			cout << "Ok send\n";
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
	unsigned long opt = 0;
	ioctlsocket(sock, FIONBIO, &opt);
	std::cout << "Adres: \"" << addr << "\"\n";
	if(connect(sock, (SOCKADDR *)(&SockAddr), sizeof(SockAddr)) == SOCKET_ERROR) 
		return WSAGetLastError();
	opt = 1;
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
	cout << "Odebralem klucz: " << klucz << "\n";
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