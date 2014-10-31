/**********************************************************************
* Title:    int.h
*
* Author:   Andrew Smythe
*
* Date:     November 29th, 2012
*
* Purpose:  Function headers for converting unsigned integers and
*           four byte character arrays.
*
* Notes:    These functions maks an assumption that the 4 byte
*           array is stored in big-endian format, and that an
*           unsigned integer is 4 bytes on the system.
*
**********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifndef INT_H
#define INT_H

/* 
    getInt -- Takes in a four byte unsigned char array, and returns a
              4 byte integer represented by it.
*/
unsigned int getInt(unsigned char*);


/*
    toBytes -- Takes in an unsigned integer, and returns a 4 byte char
               array of the bytes represented by the 4 byte integer.
*/
void toBytes(unsigned int, unsigned char*);

#endif
