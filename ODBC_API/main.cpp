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
	q.prepareQuery("SELECT * FROM klient LIMIT 10");
	test.exec(&q);
	cout << q.getString(1);
	q.next();
	cout << q.getString(2);
	cin.get();
	return 0;
}