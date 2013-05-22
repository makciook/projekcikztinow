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

int DBConnector::connect(string host, string user, string password, string db)
{
	try {
		driver = get_driver_instance();
		connection = driver->connect(host, user, password);
		connection->setSchema(db);
		result = "";
		child->encapsulate(Types::DB_CONNECT,0,result.c_str(),result.length());
	} catch (SQLException &e)
	{
		result = e.what();
		child->encapsulate(Types::DB_CONNECT,1,result.c_str(),result.length());
		return 1;
	}
	return 0;
}

int DBConnector::executeQuery(string query, unsigned int id)
{
	int cols;

	cout << "EXECUTING: "<<query<<endl;
	try {

		if (connection -> isClosed()) {
			throw runtime_error("DatabaseMetaData FAILURE - database connection closed");
		}


		statement = connection->createStatement();
		resultSet = statement->executeQuery (query);
		rsmd = resultSet->getMetaData();

		if (resultSet -> rowsCount() == 0) {
			throw runtime_error("ResultSetMetaData FAILURE - no records in the result set");
		}

		result ="";
		result += "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		result +='\n';
		result += "<row type=\"header\">";
		result +='\n';
		
		cols = rsmd->getColumnCount();


		for (int i = 1; i < cols+1; i++)
		{
			result+="<col>";
			result+=rsmd->getColumnName(i);
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


			child->encapsulate(Types::DB_EXEC,0,result.c_str(),result.length(), id);
			return 0;

	} catch (SQLException &e)
	{
		result = e.what();
		child->encapsulate(Types::DB_EXEC,1,result.c_str(),result.length(), id);
		return 1;
	}
}