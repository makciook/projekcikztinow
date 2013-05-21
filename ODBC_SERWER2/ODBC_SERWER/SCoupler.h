#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include "Md5.h"

using namespace std;

class Crypter;

class SCoupler
{
	Crypter* parent;
	SOCKET sock;
	timeval tv;
public:
	SCoupler();
	~SCoupler(void);
	void setSocket(SOCKET socket) { sock = socket; };
	/*void setParent(Crypter* cry);*/
	int waitForMessage();
	int sendMessage(const char* msg, int length);
};

