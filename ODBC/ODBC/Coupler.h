#pragma once

class Coupler
{
public:
	Coupler(void);
	~Coupler(void);
	void send(void);
	void waitForMessage(void);
	void connect(void);
	void disconnect();
};

