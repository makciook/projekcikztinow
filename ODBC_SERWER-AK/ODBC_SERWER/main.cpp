#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "SCoupler.h"
 
#define MAX_CLIENTS     20
 
using namespace std;
 
class SCoupler;

struct ClientContext
{
    HANDLE thread;
    SOCKET sock;
    int  id;
};

SOCKET          sock;

vector<ClientContext> clients;
 
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
 
DWORD WINAPI processClient (LPVOID ctx)
{
 //   ClientContext *context = (ClientContext*)ctx;
	//SCoupler* coupler = new SCoupler();

	//coupler->setSocket(context->sock);

	//char klucz[33] = "01234567890123456789012345678901";

	//for (int i = 0; i < 32; ++i)
	//{
	//	klucz[i]=rand()%9;
	//}
	//klucz[32] = '/0';
	//coupler->setKey(klucz,sizeof(klucz));
	/*send(context->sock, klucz, sizeof(klucz), 0);*/


	//while(true)
	//{
	//	if (coupler->waitForMessage() != 0)
	//	{
	//		break;
	//	}
	//}
 
	//delete coupler;
 //   context->thread = NULL;
	/*clients.erase(clients.begin()+context->id);*/
    return 0;
}
 
int main(void)
{
    WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             processConnections = 5;
    int             newId;
 
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
			if (clients.size() < MAX_CLIENTS)
			{
				ClientContext cc;
				newId = clients.size();
				clients.push_back(cc);

			}

            if (newId == -1)
            {
                cout << "Serwer nie obsluguje wiekszej ilosci klientow jednoczesnie niz " << MAX_CLIENTS << endl;
            }
            else
            {
				SCoupler coupler;							///
				coupler.setSocket(clients[newId].sock);		///
                clients[newId].sock   = client;
                clients[newId].id     = newId;
                //clients[newId].thread = CreateThread (NULL, 0, processClient, (LPVOID)&clients[newId], 0, NULL);
				clients[newId].thread = CreateThread (NULL, 0, coupler.init, (LPVOID)&clients[newId], 0, NULL);
 
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