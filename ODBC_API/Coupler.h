#ifndef COUPLER_H
#define COUPLER_H

#include <WinSock2.h>
#include <Windows.h>
#include <string>

#include "Crypter.h"

using std::string;

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
