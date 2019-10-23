#ifndef SIM808TRACKER_H
#define SIM808TRACKER_H

#include "Parameters.h"
#include <AES.h>

namespace Sim808 {
    const int TrackerBufSize = 128;
    const int ShortReadTimeout = 5000;
    class Tracker {
    private:
        const Parameters &parameters;
        SoftwareSerial cellSerial;
        AES aes;
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
        bool uploadInfo();

        bool tryReadLine(const Timeout &timeout);

        // Reads until the line equals the buffer, or the timeout is met
        // Clears readBuffer at the end
        bool tryReadLine(const __FlashStringHelper *line,
            unsigned long timeout);

        bool tryReadGPSLine();
        bool tryReadIP();
    };
}

#endif 