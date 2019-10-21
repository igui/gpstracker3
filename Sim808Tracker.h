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
        Buffer<TrackerBufSize> gpsLine;
    public:
        Tracker(const Parameters &parameters);
        void setup();
        void loop();
        void run();
    private:
        bool init();
        bool readGPS();

        // Reads until the line equals the buffer, or the timeout is met
        // Clears readBuffer at the end
        bool tryReadLine(const __FlashStringHelper *line,
            unsigned long timeout);

        // Reads until a condition is met, or the timeout is met. Leaves
        // Doesn't clear the buffer
        bool readUntil(
            bool (*condition)(const Buffer<TrackerBufSize> &input),
            unsigned long timeout);
    };
}

#endif 