#ifndef _TIMEOUT_H
#define _TIMEOUT_H

class Timeout
{
 private:
	unsigned long val;
	const bool isEnabled;
 public:
    // no timeout timer
	Timeout(); 
    // timer with a timeout
    Timeout(unsigned int timeout);
	bool expired() const;
};

#endif

