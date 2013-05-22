#include "Crypter.h"
#include "Transporter.h"

#include <iostream>


Crypter::Crypter(SCoupler* child)
{
	this->child = child;
	parent = new Transporter(this);
}


Crypter::~Crypter(void)
{

}


int Crypter::encrypt(const char* msg, int length)
{
	char* buffer = new char[length+1];
	char c;
	for(int i = 0; i < length; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	buffer[length] = '\0';
	int ret = child->sendMessage(buffer, length);
	delete [] buffer;
	return ret;
}


int Crypter::decrypt(const char* msg, int length)
{
	char* buffer = new char[length+1];
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
	return 0;
}


void Crypter::disconnect(void)
{
	//child->disconn();
}


