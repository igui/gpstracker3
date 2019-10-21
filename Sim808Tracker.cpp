#include "Sim808Tracker.h"
#include "Timeout.h"
#include <HardwareSerial.h>
#include <Arduino.h>

using namespace Sim808;

extern HardwareSerial Serial;

const unsigned long ShortReadTimeout = 2000;
const unsigned long ConnectTimeout = 60000;
const unsigned long ErrorDelay = 3000;
const unsigned long WindDownDelay = 2000;

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

bool Tracker::tryReadLine(const Timeout &timeout) {
    while(true) {
        if(cellSerial.available()) {
            char c = cellSerial.read();
            readBuffer.write(c);
            if(readBuffer.isCRLF()) {
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

 bool Tracker::tryReadGPSLine() {
    const Timeout timeout(ShortReadTimeout);
    
    while(true) {
        tryReadLine(timeout);
        Serial.print(readBuffer);

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

bool Tracker::tryReadIP() {
    const Timeout timeout(ShortReadTimeout);
    
    while(true) {
        tryReadLine(timeout);
        Serial.print(readBuffer);

        if(readBuffer.isCRLF() && readBuffer.isAnIPCRLF()) {
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

bool Tracker::uploadInfo() {
    readBuffer.reset();

    // Check if the cell serial is up
    cellSerial.println(F("AT"));
    if(!tryReadLine(F("OK"), ShortReadTimeout)) {
        return false;
    }

    // Check local IP. It's seem to be a precondition to open connections
    cellSerial.println(F("AT+CIFSR"));
    if(!tryReadIP()) {
        return false;
    }

    cellSerial.print(F("AT+CIPSTART=\"UDP\",\""));
    cellSerial.print((const __FlashStringHelper *) parameters.host);
    cellSerial.print(F("\",\""));
    cellSerial.print(parameters.port, 10);
    cellSerial.println("\"");

    if(!tryReadLine(F("CONNECT OK"), ConnectTimeout)) {
        return false;
    }

    cellSerial.print(F("AT+CIPSEND="));
    cellSerial.print(gpsLine.length(), 10);
    cellSerial.println();

    // delay 1sec 
    // TODO wait for "> " prompt 
    delay(1000);

    cellSerial.print(gpsLine);
    if(!tryReadLine(F("SEND OK"), ConnectTimeout)) {
        return false;
    }

    cellSerial.println(F("AT+CIPCLOSE"));
    if(!tryReadLine(F("CLOSE OK"), ConnectTimeout)) {
        return false;
    }

    return true;
}

void Tracker::run() {
    bool errorOnLastIteration = true;
    while(true) {
        if(errorOnLastIteration) {
            Serial.println(F("INIT"));
            if(!init()) {
                Serial.println(F("INIT PHASE FAIL"));
                delay(ErrorDelay);
                continue;
            }
        }
        
        Serial.println(F("READ GPS"));
        if(!readGPS()) {
            Serial.println(F("READ GPS PHASE FAIL"));
            delay(ErrorDelay);
            errorOnLastIteration = true;
            continue;
        }

        Serial.println(F("UPLOAD INFO"));
        if(!uploadInfo()) {
            Serial.println(F("UPLOAD INFO PHASE FAIL"));
            delay(ErrorDelay);
            errorOnLastIteration = true;
            continue;
        }

        errorOnLastIteration = false;
        delay(WindDownDelay);
    }

    Serial.println(F("PROGRAM END"));
    while(true) {}
}
