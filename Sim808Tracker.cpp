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
