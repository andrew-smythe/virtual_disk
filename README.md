# Lab 6 (Assignment 2) Readme/Notes
### Author 
Andrew Smythe
### Date 
November 16th, 2012
### Purpose 
To demonstrate how to run my virtual disk simulator.

### How to Run my Program

To run the program, run the make file. "make run" will build and
execute the program.

In my program, you can format the disk (ie. write the disk file if it
has not been written, or reset one that has already been built); you
can mount and unmount (saves the virtual disk to the file vdisk); you
can randomly fill half of the disk; and you can save and write files to
the file system.

When a user attempts to save a file onto the filesystem, they are
prompted to supply the name of the file on the local filesystem. If
there are enough blocks available to save it, then it is saved to the
filesystem (and its file number is shown to the user -- my filesystem
does not support file names.) If there are not enough blocks, then the
file is simply not written.

When a user attempts to read a file from the filesystem, they must
supply the file number, and a name of a local file to save to. If they
supply a legitimate file number, the file is written to the specified
local file on the physical disk.


### Virtual Disk Filesystem Architecture

My virtual filesystem divides the filesystem up into 4 parts:
- Superblock (contains info for number of FSM and i-nodes blocks)
- Free Space Management blocks (a bitmap that shows which blocks are
    free and used)
- I-nodes block (pointers to i-nodes)
- Data blocks

Although my program could probably be altered to allow the user to
define the size of the blocks, and number of blocks... for the purpose
of the assignment, I used 50 byte blocks, and 200 total data blocks
(206 written to disk -- 1 for Superblock, 4 for FSM, 1 for i-node
management).

On mounting the disk, the program loads the FSM and i-node management
blocks to a Disk structure.

When the user saves a file to the file system, it calculates how many 
blocks will be needed, and then creates an i-node block in the data 
blocks (simply grabs the next free block), and fills some number of 
blocks with the file's data. Then, the file number (offset from the top
of the i-node management block) is returned to the user.

When the user attempts to read a file from the file system, they supply
the file number, and the data blocks within the i-node retrieved from
the i-node block (offset from the top with the file number) are saved
into the user-supplied file.


### Notable Bugs

When a file is read off of the virtual filesystem, the resulting file
will usually be slightly bigger than the original that was written into
the VFS. When a file is read off of the filesystem, it will read all of
the blocks that contain its data, and write it to a local file. If the
last block of data is not full, it will still write all 50 bytes to the
file. 

For example, if a 1002 byte GIF is saved to the VFS, it will be
1050 bytes when it is read back to the physical disk.

-----------------------------------------------------------------------

Example Output:

************************************************
        Andrew's Virtual Disk Manager
************************************************

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
1

Disk was formatted -- ./vdisk

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
2	
	
Successfully mounted drive at ./vdisk
Press enter to continue...

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
3
	
Randomly filled half of the blocks.

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
4

Enter name of file (local) to write to disk:
smiling.gif

Wrote file to file #1.

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit

Enter file number to read:
1

Enter name of local file to write to:
asdf.gif

File #1 re-written to disk.

Written to: asdf.gif

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
6	
	
Successfully wrote disk to ./vdisk.

Options:
--------------------------------
	1 - Format Disk
	2 - Mount Disk
	3 - Randomly Fill Disk
	4 - Save file to disk
	5 - Read file from disk
	6 - Unmount Disk
	q - Quit
q

** If you open asdf.gif in a picture viewing program, you can see that
    it is identical to smiling.gif. **
