#ifndef CRYPTER_H
#define CRYPTER_H

#include "Coupler.h"

class Transporter;

class Crypter
{
	char klucz[33];
	Coupler *child;
	Transporter *parent;
public:
	Crypter(Transporter *parent);
	~Crypter(void);
	int encrypt(const char* msg, int length);
	int decrypt(const char* msg, int length);
	int connect(const char* addr, const char* msg, int length);
	void disconnect(void);
	void setKey(const char* key) { strcpy_s(klucz, key); }
};

#endif /* CRYPTER_H */
