#ifndef CRYPTER_H
#define CRYPTER_H

#include "SCoupler.h"
#include <random>
#include <time.h>

class Transporter;

class Crypter
{
	char klucz[33];
	SCoupler *child;
	Transporter *parent;
public:
	Crypter(SCoupler* child);
	~Crypter(void);
	int encrypt(const char* msg, int length);
	int decrypt(const char* msg, int length);
	int connect(const char* addr, const char* msg, int length);
	void disconnect(void);
	char* genKey();
};

#endif /* CRYPTER_H */
