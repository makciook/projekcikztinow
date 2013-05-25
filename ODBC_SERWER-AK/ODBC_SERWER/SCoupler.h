#pragma once

#include "Server.h" //////////////;
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
	static bool close;
	bool read(char* buf, int size);
	int readBytes(char* buf, int size);
	int write(char *buf, int size);
public:
	SCoupler();
	~SCoupler(void);
	void setSocket(SOCKET socket) { sock = socket; };
	int run();
	int waitForMessage();
	int sendMessage(const char* msg, int length);
	static DWORD WINAPI init (LPVOID ctx);
	static void setClose(bool cl);
};

