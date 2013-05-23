#pragma once
#include <iostream>
#include <string>
#include <stdexcept>

#include "Transporter.h"

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

class DBConnector
{
	Transporter* child;
	Driver *driver;
	Connection *connection;
	Statement *statement;
	ResultSet *resultSet;
	ResultSetMetaData *rsmd;
	string result;
public:
	DBConnector(Transporter *child);
	~DBConnector(void);
	int connect(string host, string user, string password, string db);
	int executeQuery(string query, unsigned int id);
	string getResult() { return result; }
};

