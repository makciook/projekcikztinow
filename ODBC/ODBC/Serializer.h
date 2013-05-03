#pragma once
class Serializer
{
public:
	Serializer(void);
	~Serializer(void);
	void connectToDatabase(void);
	void deserialize(void);
	void serializeResult(void);
	void executeQuery(void);
};

