#include "DBConnector.h"

DBConnector::DBConnector(void)
{
}


DBConnector::~DBConnector(void)
{
		delete resultSet;
		delete statement;
		connection -> close();
		delete connection;
}

void DBConnector::connect(string host, string user, string password, string db)
{
	try {
		driver = get_driver_instance();
		connection = driver->connect(host, user, password);
		connection->setSchema(db);
	} catch (SQLException &e)
	{
		cout<<e.getErrorCode();
	}
}

ResultSet* DBConnector::executeQuery(string query)
{
	try {
		statement = connection->createStatement();
		//resultSet = statement->executeQuery (query);
		return statement->executeQuery(query);
	} catch (SQLException &e)
	{
		cout<<e.getErrorCode();
	}
}

void DBConnector::printResult(ResultSet * rs)
{
	ResultSetMetaData *rsmd;
	rsmd = rs->getMetaData();
	int cols;
	
	while (rs->next())
	{
		cols = rsmd->getColumnCount();

		for (int i = 1; i < cols+1; i++)
		{
			//cout<<rsmd->getColumnType(i)<<endl;

			if (rsmd->getColumnType(i) == 13)
			{
					cout<< rs->getString(i) << "   ";
			} 
			else if (rsmd->getColumnType(i) == 5)
			{
					cout<< rs->getInt(i) << "   ";
			} 
		}
		cout<<endl;
	}
}