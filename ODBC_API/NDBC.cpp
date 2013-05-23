#include "NDBC.h"
#include "Query.h"


NDBC::NDBC(void)
{
	child = new Controller(this);
	connected = false;
}


NDBC::~NDBC(void)
{

}

bool NDBC::connect(string user, string pass, string db, string addr)
{
	int ret = child->serializeConnectionData(user, pass, db, addr);
	if(ret != 0)
		connected = false;
	else
		connected = true;
	return connected;
}


void NDBC::disconnect(void)
{
	child->disconnect();
	connected = false;
}


void NDBC::getConnectionState(void)
{

}


bool NDBC::commit(void)
{
	if(!connected)
		return false;
	if(child->serialize(Types::DB_COMMIT, "COMMIT") != 0)
	{
		connected = false;
		return false;
	}
	else
		return true;
}


bool NDBC::rollback(void)
{
	if(!connected)
		return false;
	if(child->serialize(Types::DB_ROLLBACK, "ROLLBACK") != 0)
	{
		connected = false;
		return false;
	}
	else
		return true;
}


bool NDBC::transaction(void)
{
	if(!connected)
		return false;
	if(child->serialize(Types::DB_TRANSACTION, "START TRANSACTION") != 0)
	{
		connected = false;
		return false;
	}
	else
		return true;
}


string NDBC::getLastError(void)
{
	if(error == "")
	{
		char error[10];
		itoa(WSAGetLastError(), error, 10);
		return string(error);
	}
	else
		return error;
}

void NDBC::updateQuery(int id, unsigned int ack, string &msg)
{
	for(int i = 0; i < queries.size(); ++i)
	{
		if(queries[i]->getId() == id)
		{
			if(ack == 0)
			{
				queries[i]->setResult(msg);
				queries[i]->setLastError("");
			}
			else
			{
				queries[i]->setResult("");
				queries[i]->setLastError(msg);
			}
			queries[i]->setValid(true);
			queries.erase(queries.begin() + i, queries.end() + i);
			break;
		}
	}
}

int NDBC::exec(Query *query)
{
	if(!connected)
		return false;
	queries.push_back(query);
	query->resetQuery();
	child->serialize(Types::DB_EXEC, query->getQuery(), query->getId());
	return 0;
}