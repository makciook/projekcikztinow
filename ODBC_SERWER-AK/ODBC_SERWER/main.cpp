#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include "SCoupler.h"
 
#define MAX_CLIENTS     20
 
using namespace std;
 
class SCoupler;

SOCKET          sock;

struct temp
{
	SOCKET sock;
	int *num;
};
 
void ShutdownServer()
{

}
 
int main(void)
{
    WSADATA         wsaData;
    SOCKADDR_IN     saddr;
    SOCKET          client;
    int             processConnections = 5;
    int             newId;
	HANDLE			threads[MAX_CLIENTS];

 
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
	cout << "Wyszlo\n";
	cin.get();
 
    WSACleanup();
	return 0;
}