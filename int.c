/**********************************************************************
* Title:    int.c
*
* Author:   Andrew Smythe
*
* Date:     November 29th, 2012
*
* Purpose:  Function implementation for integer manipulation functions.
*
* Notes:    These functions maks an assumption that the 4 byte
*           array is stored in big-endian format, and that an
*           unsigned integer is 4 bytes on the system.
*
**********************************************************************/

#include "int.h"

unsigned int getInt(unsigned char* c)
{
    unsigned int ret = 0;
    unsigned int w = 0x0;
    unsigned int x = 0x0;
    unsigned int y = 0x0;
    unsigned int z = 0x0;
    
    // Convert from hex into decimal
    w = c[0]*256*256*256;
    x = c[1]*256*256;
    y = c[2]*256;
    z = c[3];
    
    // Add each byte together
    ret = w + x + y + z;
    
    return ret;
}

void toBytes(unsigned int n, unsigned char* bytes)
{   
    // Use shifts to get each byte
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
}
