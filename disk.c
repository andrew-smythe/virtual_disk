/**********************************************************************
* Title:    disk.c
*
* Author:   Andrew Smythe
*
* Date:     November 16th, 2012
*
* Purpose:  Implementation of disk manipulation functions.
*
**********************************************************************/

// Includes
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "int.h"
#include "disk.h"

// formatDisk -- opens/creates a new virtual disk, and writes i-nodes
//               and free space management blocks to disk.
extern void formatDisk(struct Disk *d)
{
    // open file for writing -- overwrite any old disk
    d->fp = fopen("vdisk", "w+");
    
    // construct buffer, and write buffer to file
    int disksize = NBLOCKS * BLOCKSIZE;
    unsigned char * buffer = (unsigned char *)malloc(disksize*sizeof(unsigned char));
    if (buffer == NULL) {
        printf("Error allocating memory!");
        return;
    }
    
    int i =0;
    for (i = 0; i < disksize; i++)
        buffer[i] = 0;
        
    // Write buffer to disk
    fwrite(buffer, 1, disksize, d->fp);
    
    // we don't need the buffer anymore
    free(buffer);
    
    // construct a new buffer that will make a super block
    buffer = (unsigned char *)malloc (BLOCKSIZE*sizeof(unsigned char));
    if (buffer == NULL) {
        printf("Error allocating memory!");
        return;
    }
    
    // number of blocks for free space management
    buffer[FSM_BIT] = FSM;
    
    // number of i-nodes block
    buffer[INODE_BIT] = INODES;
    buffer[N_BIT] = BNUM;
    
    // Write buffer to disk
    rewind(d->fp);
    fwrite(buffer, 1, BLOCKSIZE, d->fp);
    
    // get rid of buffer
    free(buffer);
    
    // close the file
    fclose(d->fp);
    
    // Success message
    printf("Disk was formatted -- ./vdisk\n");
}

// mountDisk -- loads virtual disk from file
extern void mountDisk(struct Disk *d)
{
    // open virtualdisk for reading/writing -- do not overwrite
	d->fp = fopen("vdisk", "r+");
	
	// create a buffer that can hold one block at a time
	d->super = (unsigned char *)malloc(BLOCKSIZE*sizeof(unsigned char));
	if (d->super == NULL) {
	    printf("Error allocating memory!");
	    return;
	}
	
	// load super block into buffer
	rewind(d->fp);
	int read = fread(d->super, 1, BLOCKSIZE, d->fp);
	
	// set some flags on disk
    d->FSM_SIZE = d->super[FSM_BIT];
    d->FSM_START = BLOCKSIZE;
	d->INODES_SIZE = d->super[INODE_BIT];
	d->INODES_START = BLOCKSIZE*d->FSM_SIZE+d->FSM_START;
	d->NBLOCKS = d->super[N_BIT];
	d->DBLOCKS_START = (BLOCKSIZE*d->FSM_SIZE)+(BLOCKSIZE*d->INODES_SIZE)+d->FSM_START;
	
	// allocate d->blockStatus array
	d->blockStatus = (unsigned char *)malloc((d->FSM_SIZE*BLOCKSIZE)*sizeof(unsigned char));
	if (d->blockStatus == NULL) {
	    printf("Error allocating memory!");
	    return;
	}
	
	// get free space management blocks
	fseek(d->fp, d->FSM_START, SEEK_SET);
	read = fread(d->blockStatus, 1, d->FSM_SIZE*BLOCKSIZE, d->fp);
    if (read != d->FSM_SIZE*BLOCKSIZE) {
        perror("Could not read Free Space Management");
        printf("\nREAD: %d\n", read);
        return;
    }
    
	// allocate inodes array
	d->inodes = (unsigned char *)malloc((d->INODES_SIZE*BLOCKSIZE)*sizeof(unsigned char));
	if (d->inodes == NULL) {
	    printf("Error allocating memory!");
	    return;
	}
    
    // get i-node blocks
    fseek(d->fp, d->INODES_START, SEEK_SET);
    read = fread(d->inodes, 1, d->INODES_SIZE*BLOCKSIZE, d->fp);
    if (read != d->INODES_SIZE*BLOCKSIZE) {
        perror("Could not read I-nodes");
        printf("\nREAD: %d\n", read);
        return;
    }
    
	// allocate data blocks array
	d->dblocks = (unsigned char *)malloc((d->NBLOCKS*BLOCKSIZE)*sizeof(unsigned char));
	if (d->dblocks == NULL) {
	    printf("Error allocating memory!");
	    return;
	}
	
    // get data blocks
    fseek(d->fp, d->DBLOCKS_START, SEEK_SET);
    read = fread(d->dblocks, 1, d->NBLOCKS*BLOCKSIZE, d->fp);
    if (read != d->NBLOCKS*BLOCKSIZE) {
        perror("Could not read datablocks");
        printf("\nREAD: %d\n", read);
        return;
    }
    
    // Success message
    printf("Successfully mounted drive at ./vdisk\n");
    printf("Press enter to continue...\n");
    d->blockStatus[0] = 1;
    while (getchar() != '\n');
}

// fillBlocks -- sets roughly half of the blocks to full
extern void fillBlocks(struct Disk *d)
{
    // check if the FSM bitmap has been set
    if (d->blockStatus == NULL) {
        // allocate space for FSM bitmap
        d->blockStatus = (unsigned char *)malloc((d->FSM_SIZE*BLOCKSIZE)*sizeof(unsigned char));
	    if (d->blockStatus == NULL) {
	        printf("Error allocating memory!");
	        return;
	    }
	    // set all blocks to zero -- unused
	    int i = 0;
	    for (i = 0; i < d->FSM_SIZE*BLOCKSIZE; i++) d->blockStatus[i] = 0;
	}
	
	// seed a random number generator
	srand(time(NULL));
	
	// set some blocks to 1
	int i = 0;
	for (i = 0; i < d->FSM_SIZE*BLOCKSIZE; i++) {
	    int blockNum = rand()%BNUM;
	    if (blockNum != 0) d->blockStatus[blockNum] = 1;
	}
	d->blockStatus[0] = 1;
    // Success message
    printf("Randomly filled half of the blocks.\n");
}

// unmountDisk --  writes changes to disk, closes virtual disk file
extern void unmountDisk(struct Disk *d)
{
    // Write the disk information back to the virtual disk file,
    // starting with the super block
    rewind(d->fp);
    fwrite(d->super, 1, BLOCKSIZE, d->fp);
    
    // free space management blocks
    fwrite(d->blockStatus, 1, BLOCKSIZE*d->FSM_SIZE, d->fp);
    
    // i-node blocks
    fwrite(d->inodes, 1, BLOCKSIZE*d->INODES_SIZE, d->fp);
    
    // data blocks
    fwrite(d->dblocks, 1, BLOCKSIZE*d->NBLOCKS, d->fp);
    
    // close the file, free the disk info
    fclose(d->fp);
    free(d->super);
    free(d->blockStatus);
    free(d->inodes);
    free(d->dblocks);
    
    // Success message
    printf("Successfully wrote disk to ./vdisk.\n");
    
}

// checkFull -- checks if a block is full or empty
extern int checkFull(int blockNum, struct Disk *d)
{
    return (d->blockStatus[blockNum] == 1);
}


// setFull -- set a block to full
extern void setFull(int blockNum, struct Disk *d)
{
    d->blockStatus[blockNum] = 1;
}

// getFileSize -- get size of an open file
extern unsigned int getFileSize(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}

// writeFile -- write a file to the file system
extern void writeFile(FILE *fp, struct Disk* d, char* filename)
{
    // get the size of the file
    unsigned int fileSize = getFileSize(fp);
    
    // calculate how many blocks it will take
    unsigned int numBlocks = ceil((float)fileSize / (float)BLOCKSIZE);
    
    if (totalFree(d) < numBlocks) {
        printf("Not enough space left on disk!");
        return;
    }
    
    if (numBlocks > INODE_MAX) {
        printf("%s is too big for this filesystem!", filename);
    }
    
    // write a new inode to inode block
    int i = 0;
    int inode = 0;
    for (i = 0; i < BLOCKSIZE; i++) {
        if (d->inodes[i] == 0) {
            inode = i;
            break;
        }
    }
    
    // set pointer to new inode block
    int inodeBlock = nextBlock(d);
    d->inodes[inode] = inodeBlock;
    setFull(inodeBlock, d);
    
    // set pointer to indirect block
    unsigned int indirectBlock = nextBlock(d);
    setFull(indirectBlock, d);
    
    // write block numbers for new file into the inode block
    int blockNum = (inodeBlock*BLOCKSIZE);
    
    // set pointer to indirect block
    d->dblocks[blockNum+(BLOCKSIZE-1)] = indirectBlock;
    
    // Write the filename to the inode
    for (i = 0; i < FILENAME; i++) {
        if (filename[i] == '\0')
            break;
            
        // write a character, and increment block number
        d->dblocks[blockNum+i] = filename[i];
    }
    blockNum = blockNum + FILENAME;
    
    // get the file size in 4 bytes
    // ****ASSUMES SIZE OF INTEGER IS 4 BYTES****
    unsigned char fourByteSize[4];
    toBytes(fileSize, fourByteSize);
    
    // Write the file size to the inode
    for (i = 0; i < 4; i++) {
        d->dblocks[blockNum] = fourByteSize[i];
        blockNum++;
    }
    
    unsigned char * buffer = (unsigned char *) malloc(BLOCKSIZE*sizeof(unsigned char*));
    if (buffer == NULL) {
        printf("Out of memory.");
        return;
    }
    
    int indirectCount = 0;
    
    // begin writing block numbers, and then write file information to
    // the block (only within the inode)
    for (i = 0; i < numBlocks; i++) {
        int next = nextBlock(d);
        
        setFull(next, d);
        int read = fread(buffer, 1, BLOCKSIZE, fp);
        if (read != BLOCKSIZE && i != numBlocks-1) {
            perror("Could not read file");
            return;
        }
        int j = 0;
        int infoBlock = (next*BLOCKSIZE);
        
        if (i+INODE_DATA_SIZE < BLOCKSIZE-1) {
            // if we are not indirect
            d->dblocks[(inodeBlock*BLOCKSIZE)+(i+INODE_DATA_SIZE)] = next;
        }
        else {
            // if we are indirect
            d->dblocks[(indirectBlock*BLOCKSIZE)+indirectCount] = next;
            indirectCount++;            
        }
        for (j = 0; j < BLOCKSIZE; j++) {
            d->dblocks[infoBlock+j] = buffer[j];
        }
    }
    // Success message
    int j;
    printf("Wrote %s to the filesystem.\n", filename);
}

// readFile -- reads the nth file on the file system, return 0 if failed
extern int readFile(FILE *fp, struct Disk* d, char * filename) {
        
    // find the inode block
    int i = 0;
    int inodeBlock = 0;
    int currBlockNum = 0;
    
    // get an inode, read file name out of it -- search for file
    while (d->inodes[i] != 0) {
        int inode = d->inodes[i];
        currBlockNum = (inode*BLOCKSIZE);
        
        // get name of the file
        char fileName[FILENAME];
        int j = 0;
        for (j = 0; j < FILENAME; j++) {
            fileName[j] = d->dblocks[currBlockNum];
            currBlockNum++;
        }    
        // check if file name is the same
        if (strcmp(fileName, filename) == 0) {
            inodeBlock = inode;
            break;
        }
        
        // increment
        i++;
    } 
    
    // file was not found
    if (d->inodes[i] == 0) {
        printf("The file you specified does not exist on disk.\n");
        return 0;
    } 
    
    // current byte within block
    currBlockNum = (inodeBlock*BLOCKSIZE);
    
    // get the indirect block
    unsigned int indirectBlock = d->dblocks[currBlockNum+(BLOCKSIZE-1)];
    
    
    // get name of the file
    char fileName[FILENAME];
    for (i = 0; i < FILENAME; i++) {
        fileName[i] = d->dblocks[currBlockNum];
        currBlockNum++;
    }    
    
    // get the size of the file in bytes
    unsigned int fileSize = 0;
    unsigned char sizeBytes[4];
    for (i = 0; i < 4; i++) {
        sizeBytes[i] = d->dblocks[currBlockNum];
        currBlockNum++;
    }
    
    // convert from bytes to unsigned int
    // ****ASSUMES SIZE OF INTEGER IS 4 BYTES****
    fileSize = getInt(sizeBytes);   
    int numBlocks = ceil((float)fileSize / (float)BLOCKSIZE);
    
    // buffer for holding block information
    unsigned char * buffer = (unsigned char *)malloc(BLOCKSIZE*sizeof(unsigned char*));
    if (buffer == NULL) {
        printf("Out of memory.");
        return 0;
    }
    
    // will store start of current reading block
    int num = 0;
    int indirectCount = 0;
    currBlockNum = (inodeBlock*BLOCKSIZE)+INODE_DATA_SIZE;
    
    // read file from the filesystem, write to outside file
    for (i = 0; i < numBlocks; i++) {
        // current block
        if (i+INODE_DATA_SIZE < BLOCKSIZE-1) {
            // we are not indirect
            num = d->dblocks[currBlockNum+i];
        }
        else {
            // go indirect
            num = d->dblocks[(indirectBlock*BLOCKSIZE)+indirectCount];
            indirectCount++;
        }
        int j = 0;
        
        // check if we still have a full block left to read
        if (fileSize > BLOCKSIZE) {
            // put information into buffer
            for (j = 0; j < BLOCKSIZE; j++) {
                buffer[j] = d->dblocks[(num*BLOCKSIZE)+j];
            }
                
            // write buffer to file
            int written = fwrite(buffer, 1, BLOCKSIZE, fp);
            if (written != BLOCKSIZE && i != numBlocks-1) {
                perror("Could not write to file.");
                return 0;
            }
        }
        else {
            // we are reading less than a block
            // put information into buffer
            for (j = 0; j < fileSize; j++)
                buffer[j] = d->dblocks[(num*BLOCKSIZE)+j];
                
            // write buffer to file
            int written = fwrite(buffer, 1, fileSize, fp);
            if (written != fileSize && i != numBlocks-1) {
                perror("Could not write to file.");
                return 0;
            }
        }
        // decrement a size of blocks from the file size to keep track
        // of how much is left to read
        fileSize = fileSize - BLOCKSIZE;
    }
    // Success message
    printf("File %s was re-written to disk.\n", fileName);
    return 1;
}


// totalFree -- return number of total free blocks remaining
extern int totalFree(struct Disk* d)
{
    int count = 0;
    int i = 0;
    for (i = 0; i < NBLOCKS; i++) {
        if (d->blockStatus[i] == 0) count++;
    }
    return count;
}

// nextBlock -- returns the number of the next free block on disk
extern int nextBlock(struct Disk* d)
{
    int i = 0;
    for (i = 0; i < NBLOCKS; i++) {
        if (d->blockStatus[i] == 0) return i;
    }
    return -1; // no free blocks
}

// getListing -- gets a listing of the files on disk
extern void getListing(struct Disk* d)
{
    // heading
    printf("Files on Disk:\n");
    printf("-------------------------------------------------\n");
    
    // list all the files in the filesystem
    int i = 0;
    int j = 0;
    while (d->inodes[i] != 0)
    {                
        // get an inode, read file name out of it
        int inodeBlock = d->inodes[i];
        int currBlockNum = (inodeBlock*BLOCKSIZE);
        
        // get name of the file
        char fileName[FILENAME];
        for (j = 0; j < FILENAME; j++) {
            fileName[j] = d->dblocks[currBlockNum];
            currBlockNum++;
        }    
        
        // get the size of the file in bytes
        unsigned int fileSize = 0;
        unsigned char sizeBytes[4];
        for (j = 0; j < 4; j++) {
            sizeBytes[j] = d->dblocks[currBlockNum];
            currBlockNum++;
        }   
        
        // get file size as integer
        fileSize = getInt(sizeBytes);
        
        //display information
        printf("\t\t- %s\t%d bytes\n", fileName, fileSize);
        // go to next file
        i++;
    }
    printf("-------------------------------------------------\n");

}
