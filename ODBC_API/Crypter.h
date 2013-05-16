#ifndef CRYPTER_H
#define CRYPTER_H

#include "Coupler.h"

class Transporter;

class Crypter
{
	char klucz[32];
	Coupler *child;
	Transporter *parent;
public:
	Crypter(Transporter *parent);
	~Crypter(void);
	int encrypt(const char* msg);
	int decrypt(const char* msg);
	int connect(const char* addr, const char* msg);
	void disconnect(void);
	void setKey(const char* key) { strcpy_s(klucz, key); }
};

#endif /* CRYPTER_H */
