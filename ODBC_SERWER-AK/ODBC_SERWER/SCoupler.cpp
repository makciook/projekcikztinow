#include "SCoupler.h"
#include "Crypter.h"

bool SCoupler::close = false;

SCoupler::SCoupler()
{
	parent = new Crypter(this);

	int timeout = 1500;
    tv.tv_sec = (float)timeout/1000;
    tv.tv_usec = (timeout%1000)*1000;

}

SCoupler::~SCoupler(void)
{
	closesocket(sock);

}

int SCoupler::waitForMessage()
{
	bool resend = true;
	char *buffer;
	unsigned int size;
	int counter = 0;
	while(resend)
	{
		if(!read((char*)&size, sizeof(size)))
			return WSAGetLastError();
		size = ntohl(size);
		//cout << "Odebrano size: " << size << "\n";
		char checksum[33];
		if(!read(checksum, 32))
			return WSAGetLastError();
		checksum[32] = '\0';
		//cout << "Odebrano suma: \"" << checksum << "\"\n";
		buffer = new char[size+1];
		if(!read(buffer, size))
			return WSAGetLastError();
		buffer[size] = '\0';
		string buf(buffer, size);
		string checkSum(checksum);
		if(md5(buf) != checkSum)
		{
			++counter;
			if(counter > 2)
			{
				if(write("2", 1) != 0)
					return WSAGetLastError();
				cout << "Stopped send!\n";
				return 5;
			}
			if(write("1", 1) != 0)
				return WSAGetLastError();
			cout << "Error send\n";
			resend = true;
		}
		else
		{
			if(write("0", 1) != 0)
				return WSAGetLastError();
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
	if(msg == NULL || length < 0)
		return 1;
	string checkSum;
	int size = length;
	checkSum = md5(string(msg, length));
	int bufSize = size+checkSum.length()+sizeof(size);
	char *buffer = new char[bufSize];
	/*cout << "Wysylam size: " << size << "\n";
	cout << "wysylam suma: \"" << checkSum << "\"\n";
	cout << "Wysylam msg: \"";
	for(int i = 0; i < length; ++i)
		cout << msg[i];
	cout << "\"\n";*/
	size = htonl(size);
	memcpy(buffer, &size, sizeof(size));
	memcpy(buffer+sizeof(size), checkSum.c_str(), checkSum.length());
	memcpy(buffer+sizeof(size)+checkSum.length(), msg, length);
	bool resend = true;
	char response[2];
	int counter = 0;
	while(resend)
	{
		if(write(buffer, bufSize) != 0)
			return WSAGetLastError();
		std::cout << "Czekam na odpowiedz\n";
		if(!read(response, 1))
			return WSAGetLastError();
		response[1] = '\0';
		cout << "Odebrano: " << response << "\n";
		if(strcmp(response, "0") == 0)
			resend = false;
		else
		{
			++counter;
			if(counter > 2)											// maksymalnie 3 razy ponawiamy próbê wys³ania
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

int SCoupler::run()
{
	char klucz[32];
	memcpy(klucz, parent->genKey(), 32);
	if(send(sock, klucz, 32, 0) == SOCKET_ERROR)
	{
		return 1;
	}
	int ret;
	while(true)
	{
		if(close == true)
		{
			this->~SCoupler();
			break;
		}
		ret = this->waitForMessage();
		if ( ret == 0 || ret == WSAECONNRESET || ret == WSAENETDOWN || ret == WSAENETRESET || ret == WSAECONNABORTED
				|| ret == WSAESHUTDOWN || ret == WSAETIMEDOUT || ret == WSAECONNREFUSED || ret == WSAEHOSTDOWN)
		{
			break;
		}
	}
	cout << "Wyszlem\n";
	return 0;
}

DWORD WINAPI SCoupler::init (LPVOID ctx)
{
	SCoupler *coup = new SCoupler();
	coup->setSocket((SOCKET)ctx);
	coup->run();
	delete coup;
	return 0;
}

int SCoupler::readBytes(char* buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;

	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	int nError = select(0, &fd, NULL, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
	if (nError == 0)											// timeout
		return -2;
	else if(nError == -1)										// winsock error
		return -1;
	int dataLength = recv(sock, buf, size, 0);
	if(dataLength >= 0)
		return dataLength;
	else														// winsock error
		return -1;												
}

bool SCoupler::read(char *buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;
	int need = size;
	int received = 0;
	int s_counter = 0;
	while(need>0)
	{
		int ret = readBytes(buf+received, need);
		if(ret > 0)												// poprawnie odebrano
		{
			received += ret;
			need -= ret;
		}
		else if(ret == -2)										// timeout
		{
			//++s_counter;
			//if(s_counter > 3)
			//	return false;
		}
		else
			return false;
	}
	return true;
}

int SCoupler::write(char *buf, int size)
{
	if(buf == NULL || size < 0)
		return 1;
	fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
	int nError = select(0, NULL, &fd, NULL, &tv);				// czekamy a¿ bêdzie mo¿na coœ odczytaæ z socketa
	if (nError == 0)											// timeout
		return -2;
	else if(nError == -1)										// winsock error
		return -1;
	if(send(sock, buf, size, 0) == SOCKET_ERROR)
		return -1;
	return 0;
}

void SCoupler::setClose(bool cl)
{
	close = cl;
}