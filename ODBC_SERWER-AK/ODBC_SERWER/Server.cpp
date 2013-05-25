#include "Server.h"


Server::Server(unsigned int clients, unsigned int port)
{
	if (clients < 0)
	{
		this->clients = 10;
	}
	else 
	{
		this->clients = clients;
	}

	if (port < 0)
	{
		this->port = 27017;
	}
	else
	{
		this->port = port;
	}

	processConnections = 5;
    WSAStartup( MAKEWORD(2,2), &wsaData );
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset( (void*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(this->port);
    saddr.sin_addr.s_addr =inet_addr( "0.0.0.0" );
}


Server::~Server(void)
{
	closesocket(sock);
}

int Server::run(void)
{
    WSAStartup( MAKEWORD(2,2), &wsaData );
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset( (void*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(27017);
    saddr.sin_addr.s_addr =inet_addr( "0.0.0.0" );

	for(int i = 0; i < MAX_CLIENTS; ++i)
		threads[i] = NULL;
 
    if ( bind(sock, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR )
    {
        cout << "Wyst¹pi³ b³¹d podczas bindowania adresu!\n";
        return -1;
    }
 
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Ustawienie gniazda w tryb nas³uchiwania nie powiod³o siê\n";
        closesocket (sock);
        return -2;
    }
 
    while (1)
    {
        client = accept(sock, NULL, NULL);
 
        if (client == INVALID_SOCKET)
        {
            if (WSAGetLastError() == WSAECONNRESET)
            {
                cout << "WSAECONNRESET\n";
            }
        }
        else
		{
			newId = -1;
			for(int i = 0; i < MAX_CLIENTS; ++i)
			{
				if(threads[i] == NULL)
				{
					newId = i;
					break;
				}
				DWORD result = WaitForSingleObject( threads[i], 0);

				if (result == WAIT_OBJECT_0) {
					newId = i;
					break;
				}
			}
			if (newId == -1)
			{
                cout << "Serwer jest pelen!\n";
				//close socket
            }
            else
            {
				cout << "Przydzielilem " << newId << "\n";
				threads[newId] = CreateThread (NULL, 0, SCoupler::init, (LPVOID)client, 0, NULL);
 
                if (threads[newId] == NULL)
                {
                    cout << "Utworzenie watku dla klienta nie powiodlo sie." << endl;
                }
            }
        }
    }

	closesocket(sock);
    WSACleanup();
		
	return 0;
}
