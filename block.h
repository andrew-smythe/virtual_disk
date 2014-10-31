/**********************************************************************
* Title:    block.h
*
* Author:   Andrew Smythe
*
* Date:     November 16th, 2012
*
* Purpose:  Constants for block sizes.
*
**********************************************************************/
#ifndef BLOCK_H
#define BLOCK_H

#define BNUM 200
#define SUPERBLOCK 1
#define FSM 1
#define INODES 1

#define FSM_BIT 0
#define INODE_BIT 1
#define N_BIT 2

#define FILENAME 20

static int BLOCKSIZE = 200;
static int NBLOCKS =   BNUM + SUPERBLOCK + FSM + INODES;
static int INODE_DATA_SIZE = 25;     // BLOCKSIZE - FILENAME - sizeof(unsigned int) - 1
static int INODE_LEFTOVER = 25;      // BLOCKSIZE - INODE_DATA_SIZE
static int INODE_MAX = 75;           // BLOCKSIZE + INODE_DATA_SIZE

#endif
