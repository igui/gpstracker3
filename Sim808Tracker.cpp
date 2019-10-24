#include "Sim808Tracker.h"
#include "Timeout.h"
#include <HardwareSerial.h>
#include <Arduino.h>

using namespace Sim808;

extern HardwareSerial Serial;

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

void callFunc(void (*func)()) {
    if(func) {
        func();
    }
}

void Tracker::run() {
    bool errorOnLastIteration = true;
    while(true) {
        if(errorOnLastIteration) {
            Serial.println(F("INIT"));
            callFunc(parameters.onInit);
            if(!init()) {
                callFunc(parameters.onError);
                Serial.println(F("INIT PHASE FAIL"));
                delay(ErrorDelay);
                continue;
            }
        }
        
        Serial.println(F("READ GPS"));
        callFunc(parameters.onReadGPS);
        if(!readGPS()) {
            callFunc(parameters.onError);
            Serial.println(F("READ GPS PHASE FAIL"));
            delay(ErrorDelay);
            errorOnLastIteration = true;
            continue;
        }

        Serial.println(F("UPLOAD INFO"));
        callFunc(parameters.onTransmit);
        if(!uploadInfo()) {
            callFunc(parameters.onError);
            Serial.println(F("UPLOAD INFO PHASE FAIL"));
            delay(ErrorDelay);
            errorOnLastIteration = true;
            continue;
        }

        callFunc(parameters.onFinish);
        errorOnLastIteration = false;
        delay(WindDownDelay);
    }

    Serial.println(F("PROGRAM END"));
    while(true) {}
}
