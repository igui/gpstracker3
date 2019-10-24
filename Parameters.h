#ifndef _SIM808_PARAMETERS_H
#define _SIM808_PARAMETERS_H

#include <SoftwareSerial.h>
#include "Buffer.h"

namespace Sim808 {
    struct Parameters {
        // APN name. Must be a PROGMEM pointer
        const char *apn;
        // APN user. Must be a PROGMEM pointer
        const char *user;
        // APN password. Must be a PROGMEM pointer
        const char *password;

        // Tracker server domain (like host.example.com). 
        // Must be a PROGMEM pointer
        const char *host;

        // Tracker host port 
        unsigned long port;

        // Sim808 serial receive pin
        uint8_t receivePin;

        // Sim808 serial trasmission pin
        uint8_t transmitPin;

        void (*onInit)();
        void (*onReadGPS)();
        void (*onTransmit)();
        void (*onError)();
        void (*onFinish)();
    };
}

#endif