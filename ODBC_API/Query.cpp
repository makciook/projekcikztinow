#include "Query.h"
#include <iostream>

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
	if(curNode == NULL)
		return 0;
	xml_node<> *node = curNode->first_node("col");
	int i = col;
	while(node != NULL && i > 0)
	{
		node = node->next_sibling();
		--i;
	}
	if(i == 0 && node != NULL)
		return atoi(node->value());
	else
		return 0;
}

double Query::getDouble(int col)
{
	if(curNode == NULL)
		return 0.0;
	xml_node<> *node = curNode->first_node("col");
	int i = col;
	while(node != NULL && i > 0)
	{
		node = node->next_sibling();
		--i;
	}
	if(i == 0 && node != NULL)
		return atof(node->value());
	else
		return 0.0;
}

string Query::getString(int col)
{
	if(curNode == NULL)
		return string("");
	xml_node<> *node = curNode->first_node("col");
	int i = col;
	while(node != NULL && i > 0)
	{
		node = node->next_sibling();
		--i;
		cout << "Next\n";
	}
	if(i == 0 && node != NULL)
		return node->value();
	else
		return string("");
}

bool Query::next()
{
	if(curNode == NULL)
		return false;
	curNode = curNode->next_sibling();
	if(curNode == NULL)
		return false;
	else
		return true;
}

void Query::setResult(const string &result)
{
	this->result = result;
	if(result == "")
		return;
	doc.parse<0>((char*)this->result.c_str());								// 0 means default parse flags
	curNode = doc.first_node("data");
	if(curNode == NULL)
		return;
	curNode = curNode->first_node("row");								// row with columns headers
}

void Query::resetQuery()
{
	setValid(false);
	setResult("");
	setLastError("");
	curNode = NULL;
}