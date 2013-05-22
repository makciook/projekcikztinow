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

	try {
		statement = connection->createStatement();
		resultSet = statement->executeQuery (query);
		resultSetMetaData = resultSet->getMetaData();

		result ="";
		result += "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		result +='\n';
		result = "<row type=\"header\">";
		result +='\n';
		
		cols = resultSetMetaData->getColumnCount();


		for (int i = 1; i < cols+1; i++)
		{
			result+="<col>";
			result+=resultSetMetaData->getColumnName(i);
			result+="</col>";
			result +='\n';
		}

		result += "</row>";
		result +='\n';

			while (resultSet->next())
			{
				result += "<row>";
				result +='\n';
				for (int i = 1; i < cols+1; i++)
				{
					result+="<col>";
					result+=resultSet->getString(i);
					result+="</col>";
					result +='\n';
				}
								result += "</row>";
								result +='\n';
			}
			return result;

	} catch (SQLException &e)
	{
		cout<<e.getErrorCode();
	}
}