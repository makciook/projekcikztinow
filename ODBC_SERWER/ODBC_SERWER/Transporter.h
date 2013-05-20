#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include "Crypter.h"

class Controller;

enum Types { DB_CONNECT, DB_DISCONNECT, DB_CON_STATE, DB_TRANSACTION, DB_COMMIT, DB_ROLLBACK, DB_EXEC }; 

class Transporter
{
	Controller *parent;
	Crypter *child;
public:
	Transporter(Controller *parent);
	~Transporter(void);
	int encapsulate(Types type, const char* msg, int length);
	int decapsulate(const char* msg, int length);
	int connect(const char* addr, const char* msg, int length);
	void disconnect(void);
};

#endif /* TRANSPORTER_H */