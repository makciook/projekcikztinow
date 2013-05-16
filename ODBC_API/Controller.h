#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Transporter.h"

class NDBC;

class Controller
{
	NDBC *parent;
	Transporter *child;
public:
	Controller(NDBC *parent);
	~Controller(void);
	int executionReply(const char* msg);
	int systemReply(const char* msg);
	void serialize(Types type, string query, unsigned int id = 0);
	void deserialize(void);
	void serializeConnectionData(string user, string pass, string db, string addr);
	void disconnect(void);
};

#endif /* CONTROLLER_H */