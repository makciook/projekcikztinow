#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include "Crypter.h"
#include "DBConnector.h"


enum Types { DB_CONNECT, DB_DISCONNECT, DB_CON_STATE, DB_TRANSACTION, DB_COMMIT, DB_ROLLBACK, DB_EXEC }; 

class DBConnector;
class Crypter;

class Transporter
{
	DBConnector *parent;
	Crypter *child;
public:
	Transporter(Crypter* child);
	~Transporter(void);
	int encapsulate(Types type, int ack, const char* msg, int length, int id = 0);
	int decapsulate(const char* msg, int length);
	int connect(const char* addr, const char* msg, int length);
	void disconnect(void);
};

#endif /* TRANSPORTER_H */