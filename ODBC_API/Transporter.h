#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <string>

using std::string;

class Transporter
{
public:
	Transporter(void);
	~Transporter(void);
	void encapsulate(void);
	void decapsulate(const string &msg);
	void connect(void);
	void disconnect(void);
};

#endif /* TRANSPORTER_H */