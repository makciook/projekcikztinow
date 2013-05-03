#pragma once
class DatabaseConnector
{
public:
	DatabaseConnector(void);
	~DatabaseConnector(void);
	void establish(void);
	void execute(void);
	void sendResult(void);
};

