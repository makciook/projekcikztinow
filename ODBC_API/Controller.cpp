#include "Controller.h"
#include "NDBC.h"


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
	memcpy(&id, msg+sizeof(id), sizeof(id));
	id = ntohl(id);
	string reply(msg+sizeof(id)*2);
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


void Controller::serialize(Types type, string query, unsigned int id)
{
	if(id != 0)
	{
		char *msg = new char[query.length() + sizeof(id)];
		memcpy(msg, &id, sizeof(id));
		memcpy(msg+sizeof(id), query.c_str(), query.length());
		child->encapsulate(type, msg);
		delete [] msg;
	}
	else
	{
		child->encapsulate(type, query.c_str());
	}
	
}


void Controller::deserialize(void)
{
}


void Controller::serializeConnectionData(string user, string pass, string db, string addr)
{
	string msg;
	msg = user +";" + pass + ";" + db;
	child->connect(addr.c_str(), msg.c_str());
}

void Controller::disconnect(void)
{
	child->disconnect();
}

