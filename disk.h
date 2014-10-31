/**********************************************************************
* Title:    disk.h
*
* Author:   Andrew Smythe
*
* Date:     November 16th, 2012
*
* Purpose:  Function headers for manipulating a virtual disk.
*
**********************************************************************/

#ifndef DISK_H
#define DISK_H
#include "block.h"

// Virtual Disk Structure
struct Disk {
    // Pointer to file on Linux fs
    FILE* fp;
    
    // Superblock information -- size of FSM, i-node blocks, etc.
    int FSM_START;
    int INODES_START;
    int DBLOCKS_START;
    
    int FSM_SIZE;
    int INODES_SIZE;
    int NBLOCKS;    
    
    // Super block
    unsigned char* super;
    
    // Free Space Management blocks
    unsigned char* blockStatus;
    
    // I-node blocks
    unsigned char* inodes;
    
    // Data blocks
    unsigned char* dblocks;
};

// formatDisk -- opens/creates a new virtual disk, and writes i-nodes
//               and free space management blocks to disk.
extern void formatDisk(struct Disk *d);

// mountDisk -- loads virtual disk from file
extern void mountDisk(struct Disk *d);

// fillBlocks -- sets roughly half of the blocks to full
extern void fillBlocks(struct Disk *d);

// unmountDisk --  writes changes to disk, closes virtual disk file
extern void unmountDisk(struct Disk *d);

// checkFull -- checks if a block is full or empty
extern int checkFull(int blockNum, struct Disk *d);

// setFull -- set a block to full
extern void setFull(int blockNum, struct Disk *d);

// getFileSize -- get size of an open file
extern unsigned int getFileSize(FILE *fp);

// writeFile -- write a file to the file system
extern void writeFile(FILE *fp, struct Disk* d, char* filename);

// readFile -- reads the nth file on the file system
extern int readFile(FILE *fp, struct Disk* d, char * filename);

// totalFree -- return number of total free blocks remaining
extern int totalFree(struct Disk* d);

// nextBlock -- returns the number of the next free block on disk
extern int nextBlock(struct Disk* d);

// getListing -- gets a listing of the files on disk
extern void getListing(struct Disk* d);

#endif
