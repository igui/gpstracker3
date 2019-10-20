#include "Sim808Tracker.h"
#include "Timeout.h"
#include <HardwareSerial.h>
#include <Arduino.h>

using namespace Sim808;

extern HardwareSerial Serial;

Tracker::Tracker(const Parameters &parameters):
    parameters(parameters),
    cellSerial(parameters.receivePin, parameters.transmitPin)
{
}

void Tracker::setup() {
    cellSerial.begin(9600);
}

void Tracker::loop() {
    
}

bool Tracker::tryReadLine(
        const __FlashStringHelper *expected,
        unsigned long timeoutVal) {
    const Timeout timeout(timeoutVal);
    while(true) {
        readBuffer.tryReadLine(cellSerial, timeout);
        Serial.print(readBuffer);

        if(readBuffer.equalsCRLF(expected)) {
            readBuffer.reset();
            return true;
        }
        if(timeout.expired()) {
            Serial.println(F("EXPIRED"));
            readBuffer.reset();
            return false;
        }

        readBuffer.reset();
    }
}

bool Tracker::init() {
    readBuffer.reset();
    cellSerial.println(F("AT"));
    if(!tryReadLine(F("OK"), 2000)) {
        return false;
    }
    cellSerial.println(F("AT+CIPSHUT"));
    if(!tryReadLine(F("SHUT OK"), 2000)) {
        return false;
    }
    cellSerial.println(F("AT+CREG?"));
    if(!tryReadLine(F("OK"), 2000)) {
        return false;
    }
    cellSerial.println(F("AT+CGATT=1"));
    if(!tryReadLine(F("OK"), 2000)) {
        return false;
    }

    cellSerial.print(F("AT+CSTT=\""));
    cellSerial.print((const __FlashStringHelper *)parameters.apn);
    cellSerial.print(F("\",\""));
    cellSerial.print((const __FlashStringHelper *)parameters.user);
    cellSerial.print(F("\",\""));
    cellSerial.print((const __FlashStringHelper *)parameters.password);
    cellSerial.println(F("\""));
    if(!tryReadLine(F("OK"), 2000)) {
        return false;
    }

    cellSerial.println(F("AT+CGPSPWR=1"));
    if(!tryReadLine(F("OK"), 2000)) {
        return false;
    }

    return true;
}

void Tracker::run() {
    while(true) {
        Serial.println(F("Init"));
        if(!init()) {
            Serial.println(F("INIT PHASE FAIL"));
            delay(1000);
            continue;
        }
        break;
    }

    Serial.println(F("PROGRAM END"));
    while(true) {}
}