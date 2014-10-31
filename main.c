/**********************************************************************
* Title:    main.c
*
* Author:   Andrew Smythe
*
* Date:     November 16th, 2012
*
* Purpose:  Main routine for Virtual Filesystem Simulator. Uses an
*           ordinary file (Linux) to simulate a disk, and uses i-nodes.
*
**********************************************************************/

// Includes
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "disk.h"

// Functions for I/O
void getReadFile(struct Disk* d);
void getWriteFile(struct Disk* d);

int main(int argc, char ** argv)
{
    // Virtual disk information
    struct Disk d;
    
	printf("************************************************\n");
	printf("        Andrew's Virtual Disk Manager\n");
	printf("************************************************\n");
	
	char answer[255];
	int run = 1;
    // Run a loop for execution of the program
    while (run)
    {
	    printf("\nOptions:\n");
	    printf("--------------------------------\n");
	    printf("\t1 - Format Disk\n");
	    printf("\t2 - Mount Disk\n");
	    printf("\t3 - Randomly Fill Disk\n");
	    printf("\t4 - Save file to disk\n");
	    printf("\t5 - Get listing of disk\n");
	    printf("\t6 - Read file from disk\n");
	    printf("\t7 - Unmount Disk\n");
	    printf("\tq - Quit\n");
	    
	    scanf(" %s",answer);
		switch(answer[0]) {
			case '1': formatDisk(&d); 		break;
			case '2': mountDisk(&d);		break;
			case '3': fillBlocks(&d);     	break;
			case '4': getWriteFile(&d);     break;
			case '5': getListing(&d);       break;
			case '6': getReadFile(&d);      break;
			case '7': unmountDisk(&d);		break;
			case 'q':
			case 'Q': run = 0;              break;
			default: printf("\tPlease try again.\n");
		}
    }
    
	return 0;
}

/*
    Functions for I/O
*/

void getWriteFile(struct Disk* d)
{
    char filename[FILENAME+1];
    while(1) {
        // prompt user for a filename
        printf("\nEnter name of file (local) to write to disk (20 chars max):\n");
        scanf("%s", filename);
        
        // attempt to open file
        FILE *f = fopen(filename, "r+");
        if (f != NULL) {
            writeFile(f, d, filename);
            fclose(f);
            // end loop
            break;
        }
        printf("Could not open file. Please try again.\n");
    }

}

void getReadFile(struct Disk* d)
{
    char outfile[FILENAME];
    char filename[FILENAME];
    int filenum;
    while (1) {
        // prompt user for number of file to open
        printf("\nEnter the name of the file you would like to read? (20 chars max):\n");
        scanf("%s", outfile);
        
        // prompt user for filename
        printf("\nEnter name of local file to write to (20 chars max):\n");
        scanf("%s", filename);
        
        // attempt to write to file
        FILE *f = fopen(filename, "w+");
        if (f != NULL) {
            readFile(f, d, outfile);
            printf("\nWritten to: %s\n", filename);
            fclose(f);
            //end loop
            break;
        }
        printf("Could not open file. Please try again.\n");
    }
}
