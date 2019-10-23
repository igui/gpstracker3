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

int cipherMessage(
        AES &aes,
        Buffer<TrackerBufSize> &clearText,
        Buffer<TrackerBufSize> &cipher,
        uint8_t iv[N_BLOCK]) {
    
    // CBC initialization vector; real iv = iv x2 ex: 01234567 = 012345670123456
    const unsigned long long int myIv = 302557092;
    const int plainSize = clearText.length();

    aes.iv_inc();
    
    aes.set_IV(myIv);
    aes.get_IV(iv);

    aes.do_aes_encrypt(
        clearText.getBuf(),
        plainSize,
        cipher.getBuf(),
        (uint8_t *) ENCRYPTION_KEY,
        ENCRYPTION_KEY_BITS,
        iv);

    aes.set_IV(myIv);
    aes.get_IV(iv);

    return aes.get_size();
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

    byte iv[N_BLOCK];
    int cipherSize = cipherMessage(aes, gpsLine, readBuffer, iv);

    cellSerial.print(F("AT+CIPSEND="));
    cellSerial.print(sizeof(iv) + cipherSize, 10);
    cellSerial.println();

    // delay 1sec 
    // TODO wait for "> " prompt 
    delay(1000);

    // Write IV and cipherMessage
    cellSerial.write(iv, sizeof(iv));
    cellSerial.write(readBuffer.getBuf(), cipherSize);
    cellSerial.println();

    readBuffer.reset();
    if(!tryReadLine(F("SEND OK"), ShortReadTimeout)) {
        return false;
    }

    cellSerial.println(F("AT+CIPCLOSE"));
    if(!tryReadLine(F("CLOSE OK"), ConnectTimeout)) {
        return false;
    }

    return true;
}