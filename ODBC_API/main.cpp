#include "NDBC.h"
#include "Query.h"

#include <iostream>

using namespace std;

int main()
{
	NDBC test;
	test.connect("User", "password", "kupajasia", "127.0.0.1");
	cout << "asdasd";
	Query q;
	q.prepareQuery("SELECT * FROM szpulka LIMIT 10");
	//test.exec(&q);
	cin.get();
	return 0;
}