#ifndef CRYPTER_H
#define CRYPTER_H

#include <string>

#include "Coupler.h"
#include "Transporter.h"

using std::string;

class Crypter
{
	char klucz[32];
	Coupler *child;
	Transporter *parent;
public:
	Crypter(Transporter *parent);
	~Crypter(void);
	int encrypt(const string &msg);
	void decrypt(const string &msg);
	int connect(const string &addr, const string &msg);
	void disconnect(void);
	void setKey(const char* key) { strcpy_s(klucz, key); }
};

#endif /* CRYPTER_H */
