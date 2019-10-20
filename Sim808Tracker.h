#ifndef SIM808TRACKER_H
#define SIM808TRACKER_H

#include "Parameters.h"

namespace Sim808 {
    const int TrackerBufSize = 100;
    class Tracker {
    private:
        const Parameters &parameters;
        SoftwareSerial cellSerial;
        Buffer<TrackerBufSize> readBuffer;
    public:
        Tracker(const Parameters &parameters);
        void setup();
        void loop();
        void run();
    private:
        bool init();

        bool tryReadLine(const __FlashStringHelper *line,
            unsigned long timeout);
    };
}

#endif 