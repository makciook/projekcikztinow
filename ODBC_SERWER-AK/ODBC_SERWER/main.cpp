#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "SCoupler.h"
 
#define MAX_CLIENTS     20
 
using namespace std;
 
class SCoupler;

SOCKET          sock;

int clients_num = 0;
 
void ShutdownServer()
{
    for (int i = 0; i < clients.size(); i++)
        if (clients[i].sock != INVALID_SOCKET)
        {
            closesocket (clients[i].sock);
            clients[i].sock = INVALID_SOCKET;
        }
 
    closesocket (sock);
    sock = INVALID_SOCKET;
}

void sig_handler (int param)
{
	--clients_num;
}
 
int main(void)
{
    WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             processConnections = 5;
    int             newId;

	signal(SIGINT, sig_handler);
 
    WSAStartup( MAKEWORD(2,2), &wsaData );
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset( (void*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(27017);
    saddr.sin_addr.s_addr =inet_addr( "127.0.0.1" );
 
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
        
			if (clients_num > MAX_CLIENTS)
			{
                cout << "Serwer nie obsluguje wiekszej ilosci klientow jednoczesnie niz " << MAX_CLIENTS << endl;
            }
            else
            {
				++clients_num;
				HANDLE thread = CreateThread (NULL, 0, SCoupler::init, (LPVOID)client, 0, NULL);
 
                if (thread == NULL)
                {
                    cout << "Utworzenie watku dla klienta nie powiodlo sie." << endl;
                }
            }
        }
    }
 
    WSACleanup();
	return 0;
}