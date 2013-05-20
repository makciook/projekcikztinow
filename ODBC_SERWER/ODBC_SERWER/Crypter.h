#ifndef CRYPTER_H
#define CRYPTER_H

#include "SCoupler.h"

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
	void setKey(const char* key) { strcpy(klucz, key); }
};

#endif /* CRYPTER_H */
