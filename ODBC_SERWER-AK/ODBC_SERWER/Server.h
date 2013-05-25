#pragma once
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <vector>

#include "SCoupler.h"

using namespace std;

class Server
{
	unsigned int clients;
	unsigned int port;
	int MAX_CLIENTS;

	WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             newId;
	vector<HANDLE>	threads;

	SOCKET          sock;
	static void handler(int sig);
	static Server *instance;
public:
	Server(unsigned int clients, unsigned int port);
	~Server(void);
	int run(void);
};

