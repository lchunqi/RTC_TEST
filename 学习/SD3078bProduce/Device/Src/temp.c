#include "main.h"
#include "temp.h"
#include "soft_i2c.h"

float temp_read_value(unsigned char channel)
{
    uint16_t value = 0;
    float temp;
    unsigned char buff[2] = {0};
    
    // i2c_change_channel(0);
    if (channel == 1) {
        i2c_read_multi_bytes(TEMP_1_ADDRESS, 0, buff, 2);
    } else if (channel == 2) {
        i2c_read_multi_bytes(TEMP_2_ADDRESS, 0, buff, 2);
    } else {
        return 255;
    }
    value = buff[0];
    value = (value * 256 + buff[1]) / 16;
    if (buff[0] & 0x80) {
        temp = ((float)value - 4096) / 16;
    } else {
        temp = (float)value / 16; 
    }
    return temp;
}

