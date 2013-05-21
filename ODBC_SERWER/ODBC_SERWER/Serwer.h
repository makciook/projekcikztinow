#pragma once

#define MAX_CLIENTS     5

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#include "SCoupler.h"

using namespace std;

class SCoupler;
class Crypter;
 

class Serwer
{
	WSADATA         wsaData;
	SOCKADDR_IN     saddr;
	SOCKET          client;
	int             processConnections;
	int             newId;

	struct ClientContext
	{
		HANDLE thread;
		SOCKET sock;
		int  id;
	};

	SOCKET          sock;
	ClientContext   clients[MAX_CLIENTS];
	SCoupler		couplers[MAX_CLIENTS];

public:
	Serwer(void);
	static DWORD WINAPI processClient (LPVOID ctx);
	void running(ClientContext* ctx);
	~Serwer(void);
	int start(void);
	void shutdown(void);
};

