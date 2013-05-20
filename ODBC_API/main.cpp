#include "NDBC.h"
#include "Query.h"

#include <iostream>

using namespace std;

int main()
{
	NDBC test;
	test.connect("User", "password", "kupajasia", "127.0.0.1");
	cout << "asdasd";
	cin.get();
	return 0;
}