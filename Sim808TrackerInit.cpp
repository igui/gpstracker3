#include "Sim808Tracker.h"
#include "AES.h"

using namespace Sim808;

bool Tracker::init() {
    readBuffer.reset();
    gpsLine.reset();
    // Basic command to check if the Cell Serial works
    cellSerial.println(F("AT"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Close the active connection if that exists previously
    cellSerial.println(F("AT+CIPSHUT"));
    if(!tryReadLine(F("SHUT OK"), ShortReadTimeout)) {
        return false;
    }

    // Check if the Cell phone is registered (We can comment that)
    cellSerial.println(F("AT+CREG?"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Attach to GPRS Service (to upload data)
    cellSerial.println(F("AT+CGATT=1"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Set APN info
    cellSerial.print(F("AT+CSTT=\""));
    cellSerial.print((const __FlashStringHelper *) parameters.apn);
    cellSerial.print(F("\",\""));
    cellSerial.print((const __FlashStringHelper *) parameters.user);
    cellSerial.print(F("\",\""));
    cellSerial.print((const __FlashStringHelper *) parameters.password);
    cellSerial.println(F("\""));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Turn on GPS
    cellSerial.println(F("AT+CGPSPWR=1"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    cellSerial.println(F("AT+CIICR"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    return true;
}