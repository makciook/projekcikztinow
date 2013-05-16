#include "Query.h"

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

}

double Query::getDouble(int col)
{

}

string Query::getString(int col)
{


}

bool Query::next()
{


}