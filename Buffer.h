#ifndef SIM808BUFFER_H
#define SIM808BUFFER_H

#include "WString.h"
#include "SoftwareSerial.h"
#include "HardwareSerial.h"
#include "Timeout.h"

namespace Sim808 {
    /**
     * A buffer of fixed length
     */
    template <size_t BUF_SIZE>
    class Buffer: public Printable
    {
    private:
        char buf[BUF_SIZE];
        size_t written;
    public:
        Buffer(): written(0) { 
            memset(buf, 0, BUF_SIZE);
        };

        /**
         * Reset the buffer to an empty buffer again
         */
        void reset() {
            written = 0;
            memset(buf, 0, BUF_SIZE);
        }

        /**
         * Return true if the buffer is a full capacity
         */ 
        bool full() const { 
            return written >= BUF_SIZE;
        }

        /**
         * Return the number of written chars inside the buffer (not strlen)
         */
        size_t length() const {
            return written;
        }

        /**
         * Inserts a char at the end of the buffer, if it's not full. Returns
         * true if we written a character
         */
        bool write(char c) {
            if(full()) {
                return false;
            }
            buf[written] = c;
            written++;
            return true;
        }

        /**
         * Write the buffer into a printable object
         */
        size_t printTo(Print& p) const {
            size_t i;
            for(i = 0; i < written && i < BUF_SIZE; ++i) {
                p.write(buf[i]);
            }
            return i;
        }

        /**
         *  Returns true if the buffer ends in CRLF (\r\n)
         */ 
        bool isCRLF() const {
            return written >= 2 && buf[written-1] == '\n' && 
                buf[written - 2] == '\r';
        }

        /**
         * Returns true if the buffer equals to a string, except the CRLF ending
         */
        bool equalsCRLF(const __FlashStringHelper *_val) const {
            const char *val = (const char *)(_val);
            if(strlen_P(val) != written - 2) {
                return false;
            }
            for(size_t i = 0; i < written - 2; ++i) {
                char c = pgm_read_byte(val + i);
                if(c != buf[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         *  Attempts to read a CRLF ended from a Serial input before the 
         *  timeout expires
         */ 
        void tryReadLine(SoftwareSerial &inputSerial, const Timeout &timeout) {
            while(true) {
                if(inputSerial.available()) {
                    char c = inputSerial.read();
                    write(c);
                    if(isCRLF()) {
                        return;
                    }
                    continue;
                }

                if(timeout.expired()) {
                    return;
                }
            }
        }
    };    
}

#endif