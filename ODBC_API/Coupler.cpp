#include "Coupler.h"

Coupler::Coupler(Crypter *parent)
{
	this->parent = parent;
}


Coupler::~Coupler(void)
{

}


void Coupler::send(void)
{

}


void Coupler::waitForMessage(void)
{

}


int Coupler::conn(string addr)
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
		return 1;
	SockAddr.sin_port = htons(27015);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
	if(connect(sock, (SOCKADDR *)(&SockAddr), sizeof(SockAddr)) == SOCKET_ERROR) 
		return 2;
	char klucz[32];
	if(recv(sock, klucz, sizeof(klucz), 0) == SOCKET_ERROR)
		return 3;
	parent->setKey(klucz);
	return 0;
}


void Coupler::disconn(void)
{
}