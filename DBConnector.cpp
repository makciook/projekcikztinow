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

string DBConnector::executeQuery(string query)
{
	int cols;
	result = "";

	try {
		statement = connection->createStatement();
		resultSet = statement->executeQuery (query);
		resultSetMetaData = resultSet->getMetaData();

			while (resultSet->next())
			{
				cols = resultSetMetaData->getColumnCount();
		
				for (int i = 1; i < cols+1; i++)
				{
					//cout<<rsmd->getColumnType(i)<<endl;
		
					if (resultSetMetaData->getColumnType(i) == 13)
					{
							result+=resultSet->getString(i);
							result+=":";
					} 
					else if (resultSetMetaData->getColumnType(i) == 5)
					{
							result+=to_string(resultSet->getInt(i));
							result+=":";
					} 
				}
				result+=";";
			}
			return result;

	} catch (SQLException &e)
	{
		cout<<e.getErrorCode();
	}
}