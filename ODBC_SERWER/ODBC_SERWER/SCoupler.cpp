#include "SCoupler.h"
#include "Crypter.h"


SCoupler::SCoupler()
{
	parent = new Crypter(this);

	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;

	parent->setKey("01234567890123456789012345678901");
}

SCoupler::~SCoupler(void)
{
}

int SCoupler::waitForMessage()
{
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	bool resend = true;
	char *buffer;
	unsigned int size;
	int counter = 0;
	while(resend)
	{
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a� b�dzie mo�na co� odczyta� z socketa
		if (nError == 0)											// timeout
		{
			cout << "Timeout\n";
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			cout << "Error: " << nError << "\n";
			return 2;
		}
		int dataLength = recv(sock, (char*)&size, sizeof(size), 0);
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError: " << nError << "\n";
			return 4;
		}
		size = ntohl(size);
		cout << "Odebrano size: " << size << "\n";
		char checksum[33];
		dataLength = recv(sock, checksum, 32, 0);
		checksum[32] = '\0';
		cout << "Odebrano suma: \"" << checksum << "\"\n";
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength 2\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError 3: " << nError << "\n";
			return 4;
		}
		buffer = new char[size+1];
		dataLength = recv(sock, buffer, size, 0);
		cout << "Odebrano buf: \"";
		for(int i = 0; i < size; ++i)
			cout << buffer[i];
		cout << "\"\n";
		if (dataLength == 0)										// client disconnected
		{
			cout << "Empty datalength 3\n";
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			cout << "nError 4: " << nError << "\n";
			return 4;
		}
		buffer[size] = '\0';
		string buf(buffer, size);
		string checkSum(checksum);
		if(md5(buf) != checkSum)
		{
			++counter;
			if(counter > 2)
			{
				send(sock, "Stop\0", 5, 0);
				cout << "Stopped send!\n";
				return 5;
			}
			send(sock, "Error\0", 6, 0); 
			cout << "Error send\n";
			resend = true;
		}
		else
		{
			send(sock, "Ok\0", 3, 0); 
			cout << "Ok send\n";
			resend = false;
		}
	}
	int ret = parent->decrypt(buffer, size);
	delete [] buffer;
	return ret;
}

int SCoupler::sendMessage(const char* msg, int length)
{
	string checkSum;
	int size = length;
	checkSum = md5(string(msg, length));
	int bufSize = size+checkSum.length()+sizeof(size);
	char *buffer = new char[bufSize];
	cout << "Wysylam size: " << size << "\n";
	cout << "wysylam suma: \"" << checkSum << "\"\n";
	cout << "Wysylam msg: \"";
	for(int i = 0; i < length; ++i)
		cout << msg[i];
	cout << "\"\n";
	size = htonl(size);
	memcpy(buffer, &size, sizeof(size));
	memcpy(buffer+sizeof(size), checkSum.c_str(), checkSum.length());
	memcpy(buffer+sizeof(size)+checkSum.length(), msg, length);
	bool resend = true;
	char response[10];
	int counter = 0;
	fd_set fd;
	while(resend)
	{
		/*std::cout << "Wysylam: (len = " << bufSize;
		for(int i = 0; i < bufSize; ++i)
			std::cout << buffer[i];
		std::cout << "\n";*/
		if(send(sock, buffer, bufSize, 0) == SOCKET_ERROR)
			return 1;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
		std::cout << "Czekam na odpowiedz\n";
		int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a� b�dzie mo�na co� odczyta� z socketa
		if (nError == 0)											// timeout
		{
			return 1;
		}

		if (nError == SOCKET_ERROR)									// winsock error
		{
			return 2;
		}
		int dataLength = recv(sock, response, sizeof(response), 0);
		if (dataLength == 0)										// client disconnected
		{
			return 3;
		}
		nError = WSAGetLastError(); 
		if (nError != 0)								// winsock error
		{
			return 4;
		}
		std::cout << "Odebrano odpowiedz: \"";
		for(int i = 0; i < 10; ++i)
			cout << response[i];
		cout << "\"\n";
		if(strcmp(response, "Ok") == 0)
			resend = false;
		else
		{
			++counter;
			if(counter > 2)											// maksymalnie 3 razy ponawiamy pr�b� wys�ania
			{
				delete [] buffer;
				return 5;
			}
		}
	}
	int ret = waitForMessage();
	delete [] buffer;
	return ret;
}