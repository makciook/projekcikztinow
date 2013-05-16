#include "Crypter.h"
#include "Transporter.h"


Crypter::Crypter(Transporter *parent)
{
	child = new Coupler(this);
	this->parent = parent;
}


Crypter::~Crypter(void)
{

}


int Crypter::encrypt(const char* msg)
{
	int lim = strlen(msg);
	char* buffer;
	char c;
	for(int i = 0; i < lim; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	int ret = child->sendAndWait(buffer);
	delete [] buffer;
	return ret;
}


int Crypter::decrypt(const char* msg)
{
	int lim = strlen(msg);
	char* buffer = new char[lim];
	char c;
	for(int i = 0; i < lim; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	int ret = parent->decapsulate(buffer);
	delete [] buffer;
	return ret;
}


int Crypter::connect(const char* addr, const char* msg)
{
	int ret = child->conn(addr);
	if(ret != 0)
		return ret;
	return encrypt(msg);
}


void Crypter::disconnect(void)
{
	child->disconn();
}


