#ifndef RUGE_UTILS_H
#define RUGE_UTILS_H

namespace Utils
{
    bool IsBigEndianMachine()
    {
        union {
            unsigned int i;
            char c[4];
        } e = { 0x01000000 };

        return e.c[0];
    }
}

#endif //RUGE_UTILS_H
