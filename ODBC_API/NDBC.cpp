#include "NDBC.h"
#include "Query.h"


NDBC::NDBC(void)
{
	child = new Controller(this);
}


NDBC::~NDBC(void)
{

}

void NDBC::connect(string user, string pass, string db, string addr)
{
	child->serializeConnectionData(user, pass, db, addr);
}


void NDBC::disconnect(void)
{
	child->disconnect();
}


void NDBC::getConnectionState(void)
{

}


void NDBC::commit(void)
{
	child->serialize(Types::DB_COMMIT, "COMMIT");
}


void NDBC::rollback(void)
{
	child->serialize(Types::DB_ROLLBACK, "ROLLBACK");
}


void NDBC::transaction(void)
{
	child->serialize(Types::DB_TRANSACTION, "START TRANSACTION");
}


string NDBC::getLastError(void)
{
	return error;
}

void NDBC::updateQuery(int id, unsigned int ack, string &msg)
{


}

int NDBC::exec(Query *query)
{
	queries.push_back(query);
	child->serialize(Types::DB_EXEC, query->getQuery(), query->getId());
	return 0;
}