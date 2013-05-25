#include "NDBC.h"
#include "Query.h"

#include <iostream>

using namespace std;

int main()
{
	NDBC test;
	test.connect("root", "", "etykiety", "127.0.0.1");
	Query q;
	q.prepareQuery("SELECT * FROM szpulka LIMIT 10");
	test.exec(&q);
	cout << q.getString(1);
	q.next();
	cout << q.getString(2);
	Sleep(3000);
	q.prepareQuery("SELECT * FROM maszyna LIMIT 10");
	if(!test.exec(&q))
		cout << "Error: " << test.getLastError();
	cout << q.getString(1);
	q.next();
	cout << q.getString(2);
	cin.get();
	return 0;
}