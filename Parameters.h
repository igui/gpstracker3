#ifndef _SIM808_PARAMETERS_H
#define _SIM808_PARAMETERS_H

#include <SoftwareSerial.h>
#include "Buffer.h"

namespace Sim808 {
    struct Parameters {
        const char *apn;
        const char *user;
        const char *password;

        const char *host;
        unsigned long port;

        uint8_t receivePin;
        uint8_t transmitPin;

        void (*onInit)();
        void (*onReadGPS)();
        void (*onTransmit)();
    };
}

#endif