#include "DBConnector.h"

DBConnector::DBConnector(Transporter *child)
{
	this->child = child;
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
					result+=resultSet->getString(i);
					result+=":";
				}
				result+=";";
			}
			cout<< result;
			return result;

	} catch (SQLException &e)
	{
		cout<<e.getErrorCode();
	}
	return result;
}