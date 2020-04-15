/************************************************************** 
* Class:  CSC-415-0# Spring 2020 
* Group Name: 
* Name: 
* Student ID: 
* Name: 
* Student ID: 
* Name: 
* Student ID: 
* 
* Project: <project name, like Assignment 3 – File System 
* 
* File: <name of this file> 
* 
* Description: 
* **************************************************************/ 

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "fsLow.h"
#include "entry.h"
#include "volumeEntry.h"

void init(freeChunk freeChunks[], uint64_t lbaCount);

int main(int argc, char* argv[]) {
	uint64_t vSize = 500000;
	uint64_t* volSize = &vSize;
	char* fileName = "OurVolume";
	uint64_t bSize = 512;
	uint64_t* blockSize = &bSize;


	int status = startPartitionSystem(fileName, volSize, blockSize);
	uint64_t lbaCount = (*volSize / *blockSize) - 1;

	//The single VCB
	//freeChunk freeChunks[lbaCount];
	//extents array [lbaCount];
	//FCB array [lbaCount];
	//init(freeChunks, extentChunks, etc..,  lbaCount);
	

	/* test code
		char* buffer = "test string";

		char* result = malloc(512);
		uint64_t statusForRead = LBAread(result, 1, 99);
		printf("read status(%ld) contents:%s\n", statusForRead, result);


		uint64_t statusForWrite = LBAwrite(buffer, 1, 99);
		uint64_t statusForRead2 = LBAread(result, 1, 99);
		printf("write status(%ld) contents:%s\n", statusForWrite, buffer);
		printf("the second read status(%ld) contents:%s\n", statusForRead2, result);
		printf("status(%d) lbaCount(%ld)\n", status, lbaCount);
	*/
	closePartitionSystem();

	return 0;
}

void init(freeChunk freeChunks[], uint64_t lbaCount) {
	char* existingVolumeCheck = malloc(512);
	uint64_t statusForRead = LBAread(existingVolumeCheck, 1, 0);
	if(strcmp(existingVolumeCheck, "^") == 0) {
		printf("existing volume, read in metadata to program");
		//calculate the size of all the info in else block
		//read buffer void* and assign its values to VCB, free list, extents list, list of FCBs
	} else {
		printf("this is a new volume INIT STUFF");	
		//create volume control block and init values
		//create free list and initialize
		/*
		for(int i = 0; i < lbacount i++) {
			inuse = false;
		}
		freeChunk alpha;
		alpha.count = total lba's
		alpha.location = something close to 0

		same thing for FCB array and extent array

		*/

		//create extent list "" ""           <------ all with size lbacount 
 		//create FCB list   "" ""
 		//write to beginning of volume starting 0 ("^") or 1
 		//lbawrite with a buffer string containing all this information
	}
}