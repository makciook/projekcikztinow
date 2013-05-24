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
	if(msg == NULL || length < 0)
		return 1;
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
	char response[2];
	int counter = 0;
	while(resend)
	{
		/*std::cout << "Wysylam: (len = " << bufSize;
		for(int i = 0; i < bufSize; ++i)
			std::cout << buffer[i];
		std::cout << "\n";*/
		if(write(buffer, bufSize) != 0)
			return 1;
		std::cout << "Czekam na odpowiedz\n";
		if(!read(response, 1))
			return 2;
		response[1] = '\0';
		/*std::cout << "Odebrano odpowiedz: \"";
		for(int i = 0; i < 10; ++i)
			cout << response[i];
		cout << "\"\n";*/
		if(strcmp(response, "0") == 0)
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

int Coupler::readBytes(char* buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
	if (nError == 0)											// timeout
		return -2;
	else if(nError == -1)										// winsock error
		return -1;
	int dataLength = recv(sock, buf, size, 0);
	if(dataLength >= 0)
		return dataLength;
	else														// winsock error
		return -1;
}

bool Coupler::read(char *buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;
	int need = size;
	int received = 0;
	int s_counter = 0;
	while(need>0)
	{
		int ret = readBytes(buf+received, need);
		if(ret > 0)												// poprawnie odebrano
		{
			received += ret;
			need -= ret;
		}
		else if(ret == 0)										// timeout
		{
			++s_counter;
			if(s_counter > 3)
				return false;
		}
		else
			return false;
	}
	return true;
}

int Coupler::write(char *buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	int nError = select(0, NULL, &fd, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
	if (nError == 0)											// timeout
		return -2;
	else if(nError == -1)										// winsock error
		return -1;
	if(send(sock, buf, size, 0) == SOCKET_ERROR)
		return -1;
	return 0;
}


int Coupler::waitForMessage(void)
{
	bool resend = true;
	char *buffer;
	unsigned int size;
	int counter = 0;
	while(resend)
	{
		if(!read((char*)&size, sizeof(size)))
			return 1;
		size = ntohl(size);
		cout << "Odebrano size: " << size << "\n";
		char checksum[33];
		if(!read(checksum, 32))
			return 1;
		checksum[32] = '\0';
		cout << "Odebrano suma: \"" << checksum << "\"\n";
		buffer = new char[size+1];
		if(!read(buffer, size))
			return 1;
		buffer[size] = '\0';
		string buf(buffer, size);
		string checkSum(checksum);
		if(md5(buf) != checkSum)
		{
			++counter;
			if(counter > 2)
			{
				if(write("2", 1) != 0)
					return 2;
				cout << "Stopped send!\n";
				return 5;
			}
			if(write("1", 1) != 0)
				return 2;
			cout << "Error send\n";
			resend = true;
		}
		else
		{
			if(write("0", 1) != 0)
				return 2;
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
	if(addr == NULL)
		return 1;
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
    if(!read(klucz, 32))
		return 1;
	klucz[32] = '\0';
	parent->setKey(klucz);
	std::cout << "Ustawinono klucz " << klucz << "\n";
	return 0;
}


void Coupler::disconn(void)
{
	closesocket(sock);
}