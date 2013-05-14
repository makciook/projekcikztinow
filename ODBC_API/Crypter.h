#ifndef CRYPTER_H
#define CRYPTER_H

#include <string>

class Crypter
{
	char klucz[32];
public:
	Crypter(void);
	~Crypter(void);
	void encrypt(void);
	void decrypt(void);
	void connect(void);
	void disconnect(void);
	void setKey(const char* key) { strcpy_s(klucz, key); }
};

#endif /* CRYPTER_H */
