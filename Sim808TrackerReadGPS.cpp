#include "Sim808Tracker.h"

using namespace Sim808;

extern HardwareSerial Serial;

bool Tracker::tryReadGPSLine() {
    const Timeout timeout(ShortReadTimeout);
    
    while(true) {
        tryReadLine(timeout);
        if(readBuffer.isCRLF() && readBuffer.startsWith(F("+CGPSINF:"))) {
            return true;
        }
        if(timeout.expired()) {
            Serial.println(F("EXPIRED"));
            return false;
        }

        readBuffer.reset();
    }
}

bool Tracker::readGPS() {
    readBuffer.reset();
    gpsLine.reset();

    // Check if the cell serial is up
    cellSerial.println(F("AT"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Read GPS in format 
    cellSerial.println(F("AT+CGPSINF=32"));
    if(!tryReadGPSLine()) {
        readBuffer.reset();
        return false;
    }

    gpsLine = readBuffer;
    readBuffer.reset();
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    return true;
}