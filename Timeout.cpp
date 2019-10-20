#include "Timeout.h"
#include "Arduino.h"

Timeout::Timeout(): val(0), isEnabled(false)
{
}

Timeout::Timeout(unsigned int v): 
    isEnabled(true)
{
    val = millis() + v;
}

bool Timeout::expired() const
{
	return isEnabled && millis() > val;
}
