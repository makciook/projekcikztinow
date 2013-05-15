#ifndef COUPLER_H
#define COUPLER_H

#include <WinSock2.h>
#include <Windows.h>
#include <string>

using std::string;

class Crypter;

class Coupler
{
	SOCKET sock;
	SOCKADDR_IN SockAddr;
	Crypter *parent;
	timeval tv;
public:
	Coupler(Crypter *parent);
	~Coupler(void);
	int sendAndWait(const string &msg);
	int sendAndLeave();
	int waitForMessage(void);
	int conn(string addr);
	void disconn();
};

#endif /* COUPLER_H */
