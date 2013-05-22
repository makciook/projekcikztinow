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


	cout << "TYP ZAPYTANIA : "<<typ<<" typ connect"<<Types::DB_CONNECT<<endl;
	if (typ == Types::DB_CONNECT)
	{
		cout<<"CONNECTING TO DB:"<<endl;

		char* str = new char[size+1];
		char* pch;
		memcpy(str,msg+sizeof(typ),size);
		str[size] ='\0';

		string user = strtok (str,";");
		string pass = strtok(NULL,";");
		string db = strtok(NULL,";");
		string address = "tcp://127.0.0.1:3306";

		/*int ret = */parent->connect(address,user,pass,db);

		//string result = parent->getResult();
		//encapsulate((Types)typ,ret,result.c_str(),result.length());
	}
	else
	{
		std::cout << "Zapytanie: " << msg+sizeof(typ) << "\n";
		unsigned int id;
		memcpy(&id, msg+sizeof(typ), sizeof(id));
		id = ntohl(id);
		cout << "Odpowiadam na id: " << id << "\n";
		/*int ack = */parent->executeQuery(msg+sizeof(typ)+sizeof(id),id);
		//string result = parent->getResult();
		//encapsulate((Types)typ,ack,result.c_str(),result.length(), id);
	}
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
