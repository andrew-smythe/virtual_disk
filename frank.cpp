/* virtualdisk.cpp - program to create a virtual disk 
	
	Use an ordinary file to simulate a disk.  Ignore boot block.
	Ignore superblock (just hard code relevant info.)

	Track free blocks using byte flags (0 means free, 1 means used).
	Let block 0 hold these flags.

	Assume:
		1. Disk size  = 10,000 bytes
		2. Block Size =     50 bytes
		3. Number of blocks = 201 (200 for files and one to manage
			free blocks

	Frank LoPinto, 2012
*/

//Includes
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

//Defines
/*#define BLOCKSIZE 50
#define NBLOCKS 200 + 1*/
#define nextRand ((float)rand() / RAND_MAX)

//Globals
const int BLOCKSIZE = 50;
const int NBLOCKS = 200 + 1;
unsigned char blockStatus[NBLOCKS - 1]; 
FILE *fp = NULL;

//Prototypes
void formatDisk();  		//create linux file and fill with zeros
void mountDisk();	  		//copy block zero into array blockStatus
void randomBlockFill();	//randomly mark blocks as 'in use'
void unmountDisk();		//save blockStatus to block 0 of disk

int main() {

	printf("************************************************\n");
	printf("           Virtual Disk Manager\n");
	printf("************************************************\n");
	printf("\nSelect option:\n");
	printf("\t1 - Format Disk\n");
	printf("\t2 - Mount Disk\n");
	printf("\t3 - Random Block Fill\n");
	printf("\t4 - Unmount Disk\n");
	printf("\tq - Quit\n");

	char ans[10];
 
	int run = 1;
	while(run) {
		scanf(" %s",ans);
		switch(ans[0]) {
			case '1': formatDisk(); 		break;
			case '2': mountDisk(); 			break;
			case '3': randomBlockFill();	break;
			case '4': unmountDisk();		break;
			case 'q':
			case 'Q':  run = 0; break;
			default: printf("\tPlease try again.\n");
		}
	}
	
	return 0;
}//end main

void formatDisk() {
	fp = fopen("virtualdisk", "w+");
	int disksize = NBLOCKS * BLOCKSIZE;
	unsigned char buf[disksize];
	int i =0;
	for (i = 0; i < disksize; i++)
	    buf[i] = 'a';
	int rtn = fwrite(buf,1,disksize,fp);
	printf("Created virtualdisk.\n");
	fclose(fp);
}//end formatDisk

void mountDisk() {
	fp = fopen("virtualdisk", "r+");
	if(fp == NULL) {
		perror("");
		return;
	}
	int rtn = fread(blockStatus,1, BLOCKSIZE, fp);
	if(rtn != BLOCKSIZE) {
		perror("Can't fill blockStatus");
		return;
	}
	printf("Disk mounted. blockStatus array initialized.\n");
}// end mountDisk

void randomBlockFill() {
	//Start by marking all blocks free
	for(int i=0;i<BLOCKSIZE;i++) blockStatus[i] = '0';

	//Next, seed a random number generator using the current time.
	time_t now = time(NULL);
	srand(now);

	//Mark blocks in use.
	for(int i = 0; i<NBLOCKS/2; i++) {
		int blockNumber = nextRand * 201;
		if(blockNumber != 0) blockStatus[blockNumber-1] = '1';
	}	

	printf("Approx. half of disk randomly marked filled.\n");
}//end randomBlockFill

void unmountDisk() {
	//Write the block holding block status back to the virtual disk
	rewind(fp);
	fwrite(blockStatus,1,BLOCKSIZE,fp);
	fclose(fp);
	printf("Disk unmounted. Free BlockStatus array saved.\n");
}//end unmountDisk		

 

