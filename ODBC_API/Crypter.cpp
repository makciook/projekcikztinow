#include "Crypter.h"


Crypter::Crypter(Transporter *parent)
{
	child = new Coupler(this);
	this->parent = parent;
}


Crypter::~Crypter(void)
{

}


int Crypter::encrypt(const string &msg)
{
	string buffer;
	char c;
	for(int i = 0; i < msg.length(); ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer += c;
	}
	return child->sendAndWait(msg);
}


void Crypter::decrypt(const string &msg)
{
	string buffer;
	char c;
	for(int i = 0; i < msg.length(); ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer += c;
	}
	parent->decapsulate(buffer);
}


int Crypter::connect(const string &addr, const string &msg)
{
	if(child->conn(addr) != 0)
		return 1;
	return encrypt(msg);
}


void Crypter::disconnect(void)
{

}


