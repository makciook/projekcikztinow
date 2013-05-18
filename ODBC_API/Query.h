#ifndef QUERY_H
#define QUERY_H

#include "NDBC.h"

class Query
{
	friend class NDBC;
	unsigned int id;
	string query;
	string result;
	bool valid;
	int row;
	string error;

	static unsigned int counter;

public:
	Query();
	Query(string query);

	void prepareQuery(const string &query) { this->query = query; } 
	string getLastError() { return error; }
	int getInt(int col);
	double getDouble(int col);
	string getString(int col);
	bool isValid() { return valid; };
	bool next();


private:
	void setLastError(const string &error) { this->error = error; }
	void setResult(const string &result) { this->result = result; }
	void setValid(bool valid) { this->valid = valid; }
	string getQuery() { return this->query; }
	unsigned int getId() { return this->id; }

};

#endif /* QUERY_H */