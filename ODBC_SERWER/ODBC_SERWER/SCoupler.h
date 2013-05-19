#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include "Md5.h"

using namespace std;


class SCoupler
{
	SOCKET sock;
	timeval tv;
public:
	SCoupler();
	~SCoupler(void);
	/*void setParent(Crypter* cry);*/
	int waitForMessage(SOCKET socket);
	int sendMessage(const char* msg, int length, SOCKET socket);
};

