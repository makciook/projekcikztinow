#include "Transporter.h"
#include "Controller.h"


Transporter::Transporter(Controller *parent)
{
	this->parent = parent;
	child = new Crypter(this);
}


Transporter::~Transporter(void)
{
}


int Transporter::encapsulate(Types type, const char* msg)
{
	unsigned int typ = type;
	typ = htonl(typ);
	int len = strlen(msg);
	char *buffer = new char[len + sizeof(typ)];
	memcpy(&typ, buffer, sizeof(typ));
	memcpy((char*)msg, buffer, len);
	int ret = child->encrypt(buffer);
	delete [] buffer;
	return ret;
}


void Transporter::decapsulate(const char* msg)
{
	unsigned int typ;
	memcpy((char*)msg, &typ, sizeof(typ));
	typ = ntohl(typ);
	int size = strlen(msg)-sizeof(typ);
	char *buffer = new char[size];
	memcpy(buffer, msg+sizeof(typ),size);
	if(typ == Types::DB_EXEC)
		parent->executionReply(buffer);
	else
		parent->systemReply(buffer);
	/*if(typ == Types::DB_CONNECT)
		
	else if(typ == Types::DB_DISCONNECT)

	else if(typ == Types::DB_COMMIT)

	else if(typ == Types::DB_CON_STATE)

	else if(typ == Types::DB_EXEC)

	else if(typ == Types::DB_ROLLBACK)

	else if(typ == Types::DB_TRANSACTION)*/
	
}


int Transporter::connect(const char* addr, const char* msg)
{
	unsigned int typ = Types::DB_CONNECT;
	typ = htonl(typ);
	int len = strlen(msg);
	char *buffer = new char[len + sizeof(typ)];
	memcpy(&typ, buffer, sizeof(typ));
	memcpy((char*)msg, buffer, len);
	int ret = child->connect(addr, msg);
	delete [] buffer;
	return ret;
}


void Transporter::disconnect(void)
{
	child->disconnect();
}
