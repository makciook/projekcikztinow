#include "Crypter.h"
#include "Transporter.h"

#include <iostream>


Crypter::Crypter(Transporter *parent)
{
	child = new Coupler(this);
	this->parent = parent;
}


Crypter::~Crypter(void)
{

}


int Crypter::encrypt(const char* msg, int length)
{
	if(msg == NULL || length < 0)
		return 1;
	char* buffer = new char[length+1];
	char c;
	for(int i = 0; i < length; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	buffer[length] = '\0';
	int ret = child->sendAndWait(buffer, length);
	delete [] buffer;
	return ret;
}


int Crypter::decrypt(const char* msg, int length)
{
	if(msg == NULL || length < 0)
		return 1;
	char* buffer = new char[length];
	char c;
	for(int i = 0; i < length; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	buffer[length] = '\0';
	int ret = parent->decapsulate(buffer, length);
	return ret;
}


int Crypter::connect(const char* addr, const char* msg, int length)
{
	if(addr == NULL || msg == NULL || length <= 0)
		return 1;
	int ret = child->conn(addr);
	if(ret != 0)
	{
		std::cout << "Error: " << ret << "\n";
		return ret;
	}
	return encrypt(msg, length);
}


void Crypter::disconnect(void)
{
	child->disconn();
}


