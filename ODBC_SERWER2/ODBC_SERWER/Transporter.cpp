#include "Transporter.h"

#include <iostream>

Transporter::Transporter(Crypter* cry)
{
	this->child = cry;
	this->parent =  new DBConnector(this);
}


Transporter::~Transporter(void)
{
}


int Transporter::encapsulate(Types type, const char* msg, int length)
{
	unsigned int typ = type;
	typ = htonl(typ);
	char *buffer = new char[length + sizeof(typ)];
	memcpy(buffer, &typ, sizeof(typ));
	memcpy(buffer, msg, length);
	int ret = child->encrypt(buffer, length+sizeof(typ));
	return ret;
}


int Transporter::decapsulate(const char* msg, int length)
{
	unsigned int typ;
	memcpy(&typ, msg, sizeof(typ));
	typ = ntohl(typ);
	int size = length-sizeof(typ);
	int ret;

	string message(msg);
	cout << "Transport msg: ";
	for(int i = 0; i < length; ++i)
		cout << msg[i];
	cout << "\n";

	unsigned int dupa = 1;
	dupa = htonl(dupa);
	char * bufferdupa = new char[sizeof("Hello!")+sizeof(dupa)];
	memcpy(bufferdupa,&dupa,sizeof(dupa));
	memcpy(bufferdupa+sizeof(dupa),"Hello!",sizeof("Hello!"));

	encapsulate((Types)typ,bufferdupa,sizeof("Hello!")+sizeof(dupa));


	//parent->connect("tcp://127.0.0.1:3306","bd2","bd2","bd2-baza");
	//parent->executeQuery();
	//if(typ == Types::DB_EXEC)
	//	ret = parent->executionReply(msg+sizeof(typ));
	//else
	//	ret = parent->systemReply(msg+sizeof(typ));
	//return ret;
	return 4;
}


int Transporter::connect(const char* addr, const char* msg, int length)
{
	unsigned int typ = Types::DB_CONNECT;
	typ = htonl(typ);
	char *buffer = new char[length + sizeof(typ)+1];
	memcpy(buffer, &typ, sizeof(typ));
	memcpy(buffer+sizeof(typ), msg, length);
	buffer[length + sizeof(typ)] = '\0';
	int ret = child->connect(addr, msg, length+sizeof(typ));
	delete [] buffer;
	return ret;
}


void Transporter::disconnect(void)
{
	child->disconnect();
}
