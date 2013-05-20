#include "Crypter.h"
#include "Transporter.h"

#include <iostream>


Crypter::Crypter(SCoupler* child)
{
	this->child = child;
	parent = new Transporter(this);
	//strcpy(this->klucz, "abababababababababababababababa\0");
	setKey("abababababababababababababababa\0");
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
		//c = msg[i];
		buffer[i] = c;
	}
	buffer[length] = '\0';
	int ret = child->sendMessage(buffer, length);
	//delete [] buffer;
	return ret;




	//char* buffer = new char[length+1];
	//char c;
	//for(int i = 0; i < length; ++i)
	//{
	//	c = msg[i] ^ klucz[i%32];
	//	buffer[i] = c;
	//}
	//buffer[length] = '\0';
	//int ret = child->sendMessage(buffer, length);
	////delete [] buffer;
	//return ret;
}


int Crypter::decrypt(const char* msg, int length)
{
	//strcpy(klucz,"abababababababababababababababa");



	char* buffer = new char[length+1]; // edit
	char c;
	for(int i = 0; i < length; ++i)
		cout << msg[i];
	cout << "\n";
	cout << klucz<< "\n";
	for(int i = 0; i < length; ++i)
	{
		c = msg[i] ^ klucz[i%32];
		buffer[i] = c;
	}
	buffer[length] = '\0';
	int ret = parent->decapsulate(buffer, length);
	//delete [] buffer;
	return ret;
}


int Crypter::connect(const char* addr, const char* msg, int length)
{
	//int ret = child->conn(addr);
	//if(ret != 0)
	//{
	//	std::cout << "Error: " << ret << "\n";
	//	return ret;
	//}
	//return encrypt(msg, length);
	return 0;
}


void Crypter::disconnect(void)
{
	//child->disconn();
}


