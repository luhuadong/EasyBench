#include "gyt_common.h"

unsigned char calcCheckSum(unsigned char *data, unsigned int len)
{
    unsigned char sum = 0x00;
    while(len--) {
        sum += data[len];
    }
    return sum;
}
