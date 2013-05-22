#include "Controller.h"
#include "NDBC.h"

#include <iostream>

Controller::Controller(NDBC *parent)
{
	this->parent = parent;
	child = new Transporter(this);
}


Controller::~Controller(void)
{
}


int Controller::executionReply(const char* msg)
{
	unsigned int ack;
	memcpy(&ack, msg, sizeof(ack));
	ack = ntohl(ack);
	unsigned int id;
	memcpy(&id, msg+sizeof(ack), sizeof(id));
	id = ntohl(id);
	string reply(msg+sizeof(id)+sizeof(ack));
	parent->updateQuery(id, ack, reply);
	return 0;
}


int Controller::systemReply(const char* msg)
{
	unsigned int ack;
	memcpy(&ack, msg, sizeof(ack));
	ack = ntohl(ack);
	if(ack == 0)											// wszystko ok
		parent->setLastError("");
	else
	{
		parent->setLastError(msg+sizeof(ack));
		return 1;
	}
	return 0;
}


int Controller::serialize(Types type, string query, unsigned int id)
{
	int ret;
	if(id != 0)
	{
		unsigned int ajdi = htonl(id);
		char *msg = new char[query.length() + sizeof(ajdi)];
		memcpy(msg, &ajdi, sizeof(ajdi));
		memcpy(msg+sizeof(ajdi), query.c_str(), query.length());
		ret = child->encapsulate(type, msg, query.length() + sizeof(ajdi));
		delete [] msg;
	}
	else
	{
		ret = child->encapsulate(type, query.c_str(), query.length());
	}
	return ret;
}


void Controller::deserialize(void)
{
}


void Controller::serializeConnectionData(string user, string pass, string db, string addr)
{
	string msg;
	msg = user +";" + pass + ";" + db;
	child->connect(addr.c_str(), msg.c_str(), msg.length());
}

void Controller::disconnect(void)
{
	child->disconnect();
}

