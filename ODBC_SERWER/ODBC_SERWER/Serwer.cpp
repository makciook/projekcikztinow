#include "Serwer.h"


Serwer::Serwer(void)
{
	processConnections = 5;
}


Serwer::~Serwer(void)
{
}


DWORD WINAPI Serwer::processClient (LPVOID ctx)
{
	((Serwer*)ctx)->running((ClientContext*)ctx);
	return 0;
}

void Serwer::running(ClientContext* ctx)
{
	ClientContext *context = (ClientContext*)ctx;

	couplers[context->id].setSocket(context->sock);

	char klucz[32] = {1,0,1,0,1,1,0,1,1,1,1,0,1,0,1,1,0,1,1,1,1,0,1,0,1,1,0,1,1,1,0,1};

	send( context->sock, klucz, 32, 0 );

	while(true)
	{
		couplers[context->id].waitForMessage();
	}
	context->thread = NULL;
}



int Serwer::start(void)
{
    for (int i = 0; i < MAX_CLIENTS; i ++)
    {
        clients[i].thread = NULL;
        clients[i].sock   = INVALID_SOCKET;
        clients[i].id     = -1;
    }

    WSAStartup( MAKEWORD(2,2), &wsaData );
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset( (void*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(27015);
    saddr.sin_addr.s_addr =inet_addr( "127.0.0.1" );

    if ( bind(sock, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR )
    {
        cout << "Wyst¹pi³ b³¹d podczas bindowania adresu!\n";
        return -1;
    }
 
    if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
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
            else
                break;
        }
        else
        {
            newId = -1;
            for (int i = 0; i < MAX_CLIENTS; i ++)
                if (clients[i].sock == INVALID_SOCKET)
                {
                    newId = i;
                    break;
                }
 
            if (newId == -1)
            {
                cout << "Serwer nie obsluguje wiekszej ilosci klientow jednoczesnie niz " << MAX_CLIENTS << endl;
            }
            else
            {
                clients[newId].sock   = client;
                clients[newId].id     = newId;
				clients[newId].thread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) processClient, (LPVOID)&clients[newId], 0, NULL);

                if (clients[newId].thread == NULL)
                {
                    clients[newId].sock = INVALID_SOCKET;
                    clients[newId].id   = -1;
                    cout << "Utworzenie watku dla klienta nie powiodlo sie." << endl;
                }
            }
        }
    }
     
    HANDLE threads[MAX_CLIENTS];
    int threadsCount = 0;
     
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].thread != NULL)
        {
            threads[threadsCount] = clients[i].thread;
            threadsCount ++;
        }
         
    if (WaitForMultipleObjects(threadsCount, threads, true, 5000) == WAIT_TIMEOUT)
        for (int i = 0; i < MAX_CLIENTS; i ++)
            TerminateThread (threads[i], 2);
 
    WSACleanup();
    return 0;
}


void Serwer::shutdown(void)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].sock != INVALID_SOCKET)
        {
            closesocket (clients[i].sock);
            clients[i].sock = INVALID_SOCKET;
        }
 
    closesocket (sock);
    sock = INVALID_SOCKET;
}