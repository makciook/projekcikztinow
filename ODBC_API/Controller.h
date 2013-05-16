#ifndef CONTROLLER_H
#define CONTROLLER_H


class Controller
{
public:
	Controller(void);
	~Controller(void);
	void executionReply(const char* msg);
	void systemReply(const char* msg);
	void serialize(void);
	void deserialize(void);
	void serializeConnectionData(void);
	void disconnect(void);
};

#endif /* CONTROLLER_H */