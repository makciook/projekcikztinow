#include "Server.h"

Server* Server::instance = NULL;

Server::Server(unsigned int clients, unsigned int port)
{
	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;

	if (clients < 0)
	{
		MAX_CLIENTS = 10;
	}
	else 
	{
		MAX_CLIENTS = clients;
	}

	if (port < 0)
	{
		this->port = 27017;
	}
	else
	{
		this->port = port;
	}
	this->clients = 0;
	instance = this;

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

//void Server::handler(int sig)//
void Server::handler()//
{
	SCoupler::setClose(true);
	WaitForMultipleObjects(instance->threads.size(), instance->threads.data(), true, 10000);
	std::cout<<"KTOS MNIE CHCE POPSUC !"<<std::endl;
}

int Server::run(void)
{
    WSAStartup( MAKEWORD(2,2), &wsaData );
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset( (void*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(27017);
    saddr.sin_addr.s_addr =inet_addr( "0.0.0.0" );

	//signal(SIGILL, handler);
 
    if ( bind(sock, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR )
    {
        cout << "Wyst�pi� b��d podczas bindowania adresu!\n";
        return -1;
    }
 
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Ustawienie gniazda w tryb nas�uchiwania nie powiod�o si�\n";
        closesocket (sock);
        return -2;
    }
 
	fd_set fd;										//
	FD_ZERO(&fd);									//
	FD_SET(sock, &fd);								//

    while (1)
    {
		if (kbhit())
		{
			handler();
			break;
		}

		select(0, &fd, NULL, NULL, &tv);			//
													
			if(FD_ISSET(sock,&fd))					//
			{
				DWORD free_id = 0;
				if(threads.size() == MAX_CLIENTS)
				{
					free_id = WaitForMultipleObjects(MAX_CLIENTS, threads.data(), false, INFINITE);
					if(free_id == WAIT_FAILED)
					{
						cout << GetLastError() << "\n";
						continue;
					}
					free_id -= WAIT_OBJECT_0;
					threads.erase(threads.begin() + free_id);
				}

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
					cout << "Przydzielilem " << free_id << "\n";
					HANDLE thread = CreateThread (NULL, 0, SCoupler::init, (LPVOID)client, 0, NULL);
 
					if (thread == NULL)
						cout << "Utworzenie watku dla klienta nie powiodlo sie." << endl;
					else
						threads.push_back(thread);
				}
			}
    }

	closesocket(sock);
    WSACleanup();
		
	return 0;
}
