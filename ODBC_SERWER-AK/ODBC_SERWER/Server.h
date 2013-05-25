#pragma once
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#include "SCoupler.h"

#define MAX_CLIENTS     2

using namespace std;

class Server
{
	unsigned int clients;
	unsigned int port;

	WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             processConnections;
    int             newId;
	HANDLE			threads[MAX_CLIENTS];

	SOCKET          sock;
public:
	Server(unsigned int clients, unsigned int port);
	~Server(void);
	int run(void);
};

