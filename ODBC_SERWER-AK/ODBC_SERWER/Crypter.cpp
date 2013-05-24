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
	if (msg == NULL || length < 0)
			return 1;

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
	if (msg == NULL || length < 0)
		return 1;

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
	//child->disconnect();
}

char* Crypter::genKey()
{
	srand(time(NULL));
	memset(klucz, 0, 32);
	char possibilities[60] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoprtuwxyz";
	for(int i = 0; i < 32; ++i)
		klucz[i] = possibilities[rand() % 59];
	return klucz;
}


