#ifndef NDBC_H
#define NDBC_H

#include "Controller.h"

#include <vector>

class Query;

using namespace std;

class NDBC
{
	friend class Controller;
	vector<Query*> queries;
	Controller *child;
	string error;
	bool connected;
public:
	NDBC(void);
	~NDBC(void);
	bool connect(string user, string pass, string db, string addr);
	void disconnect(void);
	void getConnectionState(void);
	bool commit(void);
	bool rollback(void);
	bool transaction(void);
	string getLastError(void);
	bool exec(Query *query);

private:
	void setLastError(const string &error) { this->error = error; }
	void updateQuery(int id, unsigned int ack, string &msg);
};

#endif /* NDBC_H */