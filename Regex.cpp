#include "Regex.h"

bool isAnIP(const char *str, size_t maxlen) {
    int ipDots = 0;
    int ipDigits = 0;
    size_t len = strnlen(str, maxlen);
    for(unsigned int i = 0; i < len; ++i) {
        char c = str[i];
        if(isDigit(c)) {
            ++ipDigits;
            if(ipDigits > 3) {
                // too many digits in one part 1234.38954.4343.32232
                return false;
            }
        }
        else if(c == '.') {
            ++ipDots;
            ipDigits = 0;
            if(ipDots > 3) {
                // Too many dots 1.2.3.4.5
                return false;
            }
        }
    }

    return ipDots == 3;
}