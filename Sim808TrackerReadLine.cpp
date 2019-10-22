#include "Sim808Tracker.h"

using namespace Sim808;

extern HardwareSerial Serial;

bool Tracker::tryReadLine(const Timeout &timeout) {
    while(true) {
        if(cellSerial.available()) {
            char c = cellSerial.read();
            readBuffer.write(c);
            Serial.write(c);
            if(readBuffer.isCRLF() || readBuffer.full()) {
                return true;
            }
            continue;
        }

        if(timeout.expired()) {
            return false;
        }
    }
}


bool Tracker::tryReadLine(
        const __FlashStringHelper *expected,
        unsigned long timeoutVal) {
    const Timeout timeout(timeoutVal);
    
    while(true) {
        tryReadLine(timeout);

        if(readBuffer.equalsCRLF(expected)) {
            readBuffer.reset();
            return true;
        }
        if(timeout.expired()) {
            Serial.println(F("EXPIRED"));
            readBuffer.debug();
            readBuffer.reset();
            return false;
        }

        readBuffer.reset();
    }
}