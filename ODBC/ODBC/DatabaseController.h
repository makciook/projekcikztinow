#pragma once
class DatabaseController
{
public:
	DatabaseController(void);
	~DatabaseController(void);
	void executionReply(void);
	void systemReply(void);
	void serialize(void);
	void deserialize(void);
	void serializeConnectionData(void);
	void disconnect(void);
};

