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

int init(volumeEntry* vcb, char bitMap[], entry entries[], uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName);

int main(int argc, char* argv[]) {
	uint64_t vSize = 500000;
	uint64_t* volSize = &vSize;
	char* fileName = "OurVolume";
	uint64_t bSize = 512;
	uint64_t* blockSize = &bSize;


	int status = startPartitionSystem(fileName, volSize, blockSize);
	uint64_t lbaCount = (*volSize / *blockSize) - 1;

	//metadata
	volumeEntry* vcb;
	char bitMap[lbaCount];
	entry entries[lbaCount];
	int numMetadataBlocks = init(vcb, bitMap, entries, *volSize, *blockSize, lbaCount, fileName);
	printf("The number of blocks used for metadata is %d blocks out of %ld blocks.", numMetadataBlocks, lbaCount);

	//after init works main loop would go here prompting for commands
	
	closePartitionSystem();

	return 0;
}

//the params here will have to be assigned to whatever already exists on the volume if it isn't a new volume
int init(volumeEntry* vcb, char bitMap[], entry entries[], uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName) {
	//int blocksNeeded = #; //calculate after figturing out init block size for all metadata
	int trackPosition = 0; 
	volumeEntry* buffer = malloc(blockSize);
	uint64_t statusForRead = LBAread(buffer, 1, 0);

	if(buffer->startOfVolume == '^') {
		printf("existing volume, read in metadata to program\n");

		int numBlocks = 2;
		//calculate the size of all the info in else block
		//read buffer void* and assign its values to VCB, free list, usedChunk list, list of FCBs
				
				//outping all contents on volume as a check	
				//starting with vcb + existing volume character ^		
				printf("%c\n", buffer->startOfVolume);
				printf("%d\n", buffer->numLba);
				printf("%s\n", buffer->volumeName);
				printf("%d\n", buffer->volumeID);
				printf("%d\n", buffer->lbaSize);

				//out entry array followed by bitmap array values for testing here


	} else {
		printf("this is a new volume INIT STUFF\n");	

		//create volume control block and init values
		volumeEntry vcb;
		vcb.startOfVolume = '^';
		vcb.numLba = lbaCount;
		//vcb.freeInfoLocation = #;
		//vcb.rootDirLocation = #;
		strcpy(vcb.volumeName, fileName);
		vcb.volumeID = 0;
		vcb.lbaSize = blockSize;

		volumeEntry* vcbBuf = &vcb;
		LBAwrite(vcbBuf, 1, trackPosition);
		trackPosition += 1; //vcb is always < 1 full block

		//this test confirms vcb was written properly
		// volumeEntry* testbuffer = malloc(blockSize);
		// int blocksread = LBAread(testbuffer, 1, 0);
		// printf("%c\n", testbuffer->startOfVolume);
		// printf("%d\n", testbuffer->numLba);
		// printf("%s\n", testbuffer->volumeName);
		// printf("%d\n", testbuffer->volumeID);
		// printf("%d\n", testbuffer->lbaSize);
		
		printf("size of empty entry array is %ld blocks\n", (((sizeof(entry) * lbaCount) / blockSize) + 1));
		
		//write an empty entry array of the max possible entry size to the new volume
		entry* entryListBuf = entries;
		int entryListBlksWritten = LBAwrite(entryListBuf, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
		trackPosition += entryListBlksWritten;



		//write bitMap array to volume
		for(int i = 0; i < lbaCount; i++) {
			if(i < (trackPosition + (((sizeof(entry) * lbaCount) / blockSize) + 1)))
				bitMap[i] = '1'; //all the blocks used for metadata are in use
			else 
				bitMap[i] = '0'; //remainder are free
		}

		char* bitMapBuf = bitMap;
		int bitMapBlksWritten = LBAwrite(bitMapBuf, (((sizeof(char) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition+= bitMapBlksWritten;
		printf("size of empty bitMap array is %ld blocks\n", (((sizeof(char) * lbaCount) / blockSize) + 1));


		//TODO test if the volume contains the write values for bitmap and entries array
	}
	

	free(buffer);


	return trackPosition;
	
}
