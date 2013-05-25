#include <iostream>
#include "Server.h"

int main(void)
{
	Server server(2,27017);
	server.run();
	return 0;
}