#pragma once
class API
{
public:
	API(void);
	~API(void);
	void connect(void);
	void disconnect(void);
	void getConnectionState(void);
	void commit(void);
	void rollback(void);
	void transaction(void);
	void getLastError(void);
};

