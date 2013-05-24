#include "NDBC.h"
#include "Query.h"

#include <iostream>

using namespace std;

int main()
{
	NDBC test;
	test.connect("root", "", "etykiety", "127.0.0.1");
	Query q;
	q.prepareQuery("SELECT * FROM szpulka LIMIT 0");
	test.exec(&q);
	cout << q.getString(1);
	q.next();
	cout << q.getString(2);
	cin.get();
	return 0;
}