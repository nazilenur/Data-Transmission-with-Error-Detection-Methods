#include "utils.h"
#include <stdio.h>


unsigned short calculate_crc16(const char* data, int length) {
    unsigned short crc = 0xFFFF;
    int i, j;
    for (i = 0; i < length; i++) {
        crc ^= (unsigned char)data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
            else crc = (crc >> 1);
        }
    }
    return crc;
}


int calculate_parity(const char* data, int length) {
    int ones = 0;
    int i, j;
    
    for(i = 0; i < length; i++) {
        char c = data[i];
        for(j = 0; j < 8; j++) {
          
            if ((c >> j) & 1) {
                ones++;
            }
        }
    }
    return (ones % 2); 
}