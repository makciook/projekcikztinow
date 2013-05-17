#pragma once
#include <iostream>
#include <string>
#include <stdexcept>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

class DBConnector
{
	Driver *driver;
	Connection *connection;
	Statement *statement;
	ResultSet *resultSet;
public:
	DBConnector(void);
	~DBConnector(void);
	void connect(string host, string user, string password, string db);
	ResultSet* executeQuery(string query);
	void printResult(ResultSet * rs);
};

