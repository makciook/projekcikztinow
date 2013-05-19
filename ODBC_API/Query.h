#ifndef QUERY_H
#define QUERY_H

#include "NDBC.h"
#include "RapidXml\rapidxml.hpp"
#include "RapidXml\rapidxml_iterators.hpp"
#include "RapidXml\rapidxml_print.hpp"
#include "RapidXml\rapidxml_utils.hpp"

using namespace rapidxml;

class Query
{
	friend class NDBC;
	unsigned int id;
	string query;
	string result;
	bool valid;
	int row;
	string error;
	xml_document<> doc;								// character type defaults to char
	xml_node<> *curNode;

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
	void setResult(const string &result);
	void setValid(bool valid) { this->valid = valid; }
	string getQuery() { return this->query; }
	unsigned int getId() { return this->id; }
	void resetQuery();

};

#endif /* QUERY_H */