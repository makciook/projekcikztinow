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
	ResultSetMetaData *resultSetMetaData;
	string result;
public:
	DBConnector(void);
	~DBConnector(void);
	void connect(string host, string user, string password, string db);
	string executeQuery(string query);
};

