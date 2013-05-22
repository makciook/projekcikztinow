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


int Transporter::encapsulate(Types type, int ack, const char* msg, int length, int id)
{
	unsigned int typ = type;
	typ = htonl(typ);
	unsigned int ackn = htonl(ack);
	char *buffer = new char[length + sizeof(typ)];
	memcpy(buffer, &typ, sizeof(typ));
	memcpy(buffer + sizeof(typ), &ackn, sizeof(ackn));
	int size;
	if(type == Types::DB_EXEC)
	{
		int ajdi = id;
		ajdi = htonl(ajdi);
		memcpy(buffer + sizeof(typ) + sizeof(ackn), &ajdi, sizeof(ajdi));
		size = sizeof(typ) + sizeof(ackn) + sizeof(ajdi);
	}
	else
		size = sizeof(typ) + sizeof(ackn);
	memcpy(buffer+size, msg, length);
	int ret = child->encrypt(buffer, length+size);
	return ret;
}


int Transporter::decapsulate(const char* msg, int length)
{
	unsigned int typ;
	memcpy(&typ, msg, sizeof(typ));
	typ = ntohl(typ);
	int size = length-sizeof(typ);
	int ret;

	//string message(msg);
	//cout << "Transport msg: ";
	//for(int i = 0; i < length; ++i)
	//	cout << msg[i];
	//cout << "\n";dsds

	//unsigned int dupa = 1;
	//dupa = htonl(dupa);
	//char * bufferdupa = new char[sizeof("Hello!")+sizeof(dupa)];
	//memcpy(bufferdupa,&dupa,sizeof(dupa));
	//memcpy(bufferdupa+sizeof(dupa),"Hello!",sizeof("Hello!"));

	//encapsulate((Types)typ,bufferdupa,sizeof("Hello!")+sizeof(dupa));dsds

	cout << "TYP ZAPYTANIA : "<<typ<<" typ connect"<<Types::DB_CONNECT<<endl;
	if (typ == Types::DB_CONNECT)
	{
		cout<<"CONNECTING TO DB"<<endl;
		int ret = parent->connect("tcp://127.0.0.1:3306","bd23","bd2","bd2-baza");
		string result = parent->getResult();
		encapsulate((Types)typ,ret,result.c_str(),result.length());
	}
	else
	{
		std::cout << "Zapytanie: " << msg+sizeof(typ) << "\n";
		unsigned int id;
		memcpy(&id, msg+sizeof(typ), sizeof(id));
		id = ntohl(id);
		cout << "Odpowiadam na id: " << id << "\n";
		int ack = parent->executeQuery(msg+sizeof(typ)+sizeof(id));
		string result = parent->getResult();
		encapsulate((Types)typ,ack,result.c_str(),result.length(), id);
	}


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
