#include "Query.h"

unsigned int Query::counter = 0;

Query::Query()
{
	++counter;
	this->id = counter;
	query = "";
	valid = false;
}

Query::Query(string query)
{
	++counter;
	this->id = counter;
	this->query = query;
	valid = false;
}

int Query::getInt(int col)
{
	return 0;
}

double Query::getDouble(int col)
{
	return 0.0;
}

string Query::getString(int col)
{

	return "";
}

bool Query::next()
{

	return false;
}