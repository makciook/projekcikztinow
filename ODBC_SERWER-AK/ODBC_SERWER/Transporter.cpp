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


int Transporter::encapsulate(Types type, unsigned int ack, const char* msg, int length, unsigned int id)
{
	if (msg == NULL || length < 0)
		return 1;

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
	if (msg == NULL || length < 0)
		return 1;

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

		string user = "";
		int i = 0;
		while(str[i] != ';')
		{
			user.push_back(str[i]);
			++i;
		}
		++i;
		string pass = "";
		while(str[i] != ';')
		{
			pass.push_back(str[i]);
			++i;
		}
		++i;
		string db = "";
		while(str[i] != '\0')
		{
			db.push_back(str[i]);
			++i;
		}
		++i;
		string address = "tcp://127.0.0.1:3306";
		parent->connect(address,user,pass,db);
	}
	else if (typ == Types::DB_EXEC)
	{
		std::cout << "Zapytanie: " << msg+sizeof(typ) << "\n";
		unsigned int id;
		memcpy(&id, msg+sizeof(typ), sizeof(id));
		id = ntohl(id);
		cout << "Odpowiadam na id: " << id << "\n";
		parent->executeQuery(msg+sizeof(typ)+sizeof(id),(Types)typ,id);
	}
	else 
	{
		std::cout<<"Inny typ"<<endl;
		parent->executeQuery(msg+sizeof(typ),(Types)typ, 0);
	}
	return 4;
}


int Transporter::connect(const char* addr, const char* msg, int length)
{
	if (addr == NULL || msg == NULL || length <= 0)
		return 1;

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
