#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#include "SCoupler.h"
 
#define MAX_CLIENTS     20
 
using namespace std;
 
class SCoupler;

// kontekst klienta
struct ClientContext
{
    HANDLE thread;
    SOCKET sock;
    int  id;
};
 
// socket serwera
SOCKET          sock;
// klienci
ClientContext   clients[MAX_CLIENTS];
SCoupler		couplers[MAX_CLIENTS];
 
void ShutdownServer()
{
    // zamykamy niezamkniête socket'y klientów
    for (int i = 0; i < MAX_CLIENTS; i++)
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
    ClientContext *context = (ClientContext*)ctx;

	couplers[context->id].setSocket(context->sock);

	char klucz[33] = "01234567890123456789012345678901";
	send(context->sock, klucz, sizeof(klucz), 0);

	while(true)
	{
		if (couplers[context->id].waitForMessage() != 0)
			break;
	}
 
    context->thread = NULL;
    return 0;
}
 
int main(void)
{
    WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             processConnections = 5;
    int             newId;
 
    // zerujemy konteksty klientów
    for (int i = 0; i < MAX_CLIENTS; i ++)
    {
        clients[i].thread = NULL;
        clients[i].sock   = INVALID_SOCKET;
        clients[i].id     = -1;
    }
 
    // uruchamiamy serwer, to juz umiemy
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
        // akceptujemy nowego klienta
        client = accept(sock, NULL, NULL);
 
        if (client == INVALID_SOCKET)
        {
            // je¿eli dostajemy WSAENOTSOCKET to znaczy, ze ktorys z klientow zamknal
            // naszego nasluchujacego socketa
            if (WSAGetLastError() == WSAECONNRESET)
            {
                cout << "WSAECONNRESET\n";
            }
            else
                break;
        }
        else
        {
            // szukamy wolnego slota
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
                // dodajemy nowego klienta i startujemy w¹tek
                clients[newId].sock   = client;
                clients[newId].id     = newId;
                clients[newId].thread = CreateThread (NULL, 0, processClient, (LPVOID)&clients[newId], 0, NULL);
 
                // utworzenie w¹tku nie powiod³o siê
                if (clients[newId].thread == NULL)
                {
                    clients[newId].sock = INVALID_SOCKET;
                    clients[newId].id   = -1;
                    cout << "Utworzenie watku dla klienta nie powiodlo sie." << endl;
                }
            }
        }
    }
     
    // serwer zakonczyl dzialanie, tworzymy tablice uchwytów w¹tków
    HANDLE threads[MAX_CLIENTS];
    int threadsCount = 0;
     
    // uzupe³niamy tablicê
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].thread != NULL)
        {
            threads[threadsCount] = clients[i].thread;
            threadsCount ++;
        }
         
    // czekamy 5 sekund i w ostatecznosci zabijamy watki
    if (WaitForMultipleObjects(threadsCount, threads, true, 5000) == WAIT_TIMEOUT)
        for (int i = 0; i < MAX_CLIENTS; i ++)
            TerminateThread (threads[i], 2);
 
    WSACleanup();
	return 0;
}