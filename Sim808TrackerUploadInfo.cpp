#include "Sim808Tracker.h"
#include "config.h"

using namespace Sim808;

extern HardwareSerial Serial;

const unsigned long ConnectTimeout = 60000;

bool Tracker::tryReadIP() {
    const Timeout timeout(ShortReadTimeout);
    
    while(true) {
        tryReadLine(timeout);
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

bool Tracker::uploadInfo() {
    readBuffer.reset();
    aes.iv_inc();

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

    aes.do_aes_encrypt(
        readBuffer.getBuf(),
        TrackerBufSize, 
        cipherMessage,
        ENCRYPTION_KEY,
        ENCRYPTION_KEY_BITS);

    cellSerial.print(F("AT+CIPSEND="));
    cellSerial.print(sizeof(cipherMessage), 10);
    cellSerial.println();

    // delay 1sec 
    // TODO wait for "> " prompt 
    delay(1000);

    cellSerial.write(cipherMessage, sizeof(cipherMessage));
    cellSerial.println();
    if(!tryReadLine(F("SEND OK"), ShortReadTimeout)) {
        return false;
    }

    cellSerial.println(F("AT+CIPCLOSE"));
    if(!tryReadLine(F("CLOSE OK"), ConnectTimeout)) {
        return false;
    }

    return true;
}