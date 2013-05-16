#ifndef NDBC_H
#define NDBC_H


class NDBC
{
public:
	NDBC(void);
	~NDBC(void);
	void connect(void);
	void disconnect(void);
	void getConnectionState(void);
	void commit(void);
	void rollback(void);
	void transaction(void);
	void getLastError(void);
};

#endif /* NDBC_H */