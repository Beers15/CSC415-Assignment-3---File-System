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
	volumeEntry* vcb = malloc(*blockSize);
	char bitMap[lbaCount];
	entry entries[lbaCount];
	int numMetadataBlocks = init(vcb, bitMap, entries, *volSize, *blockSize, lbaCount, fileName);
	printf("The number of blocks used for metadata is %d blocks out of %ld blocks.\n", numMetadataBlocks, lbaCount);

	// Testing to see if vcb works here
	printf("VCB Start of Volume: %c\n", vcb->startOfVolume);
	printf("VCB Num LBA:		 %d\n", vcb->numLba);
	printf("VCB Volume Name:	 %s\n", vcb->volumeName);
	printf("VCB Volume ID:		 %d\n", vcb->volumeID);
	printf("VCB LBA Size:		 %d\n", vcb->lbaSize);

	//after init works main loop would go here prompting for commands
	
	closePartitionSystem();

	return 0;
}

//the params here will have to be assigned to whatever already exists on the volume if it isn't a new volume
int init(volumeEntry* vcb, char bitMap[], entry entries[], uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName) {
	//int blocksNeeded = #; //calculate after figturing out init block size for all metadata
	int trackPosition = 0; 
	uint64_t statusForRead = LBAread(vcb, 1, 0);

	if(vcb->startOfVolume == '^') {
		trackPosition += 1; //vcb is always < 1 full block
		printf("existing volume, read in metadata to program\n");
		char* bitMapBuf = malloc(((((sizeof(entry) * lbaCount) / blockSize) + 1) * blockSize));
		uint64_t statusForRead2 = LBAread(bitMapBuf, (((sizeof(entry) * lbaCount) / blockSize) + 1), trackPosition);

		int numBlocks = 2;
		//calculate the size of all the info in else block
		//read buffer void* and assign its values to VCB, free list, usedChunk list, list of FCBs
				
				//outping all contents on volume as a check	
				//starting with vcb + existing volume character ^		
				printf("Start of Volume: %c\n", vcb->startOfVolume);
				printf("Num LBA:		 %d\n", vcb->numLba);
				printf("Volume Name:	 %s\n", vcb->volumeName);
				printf("Volume ID:		 %d\n", vcb->volumeID);

				//out entry array followed by bitmap array values for testing here
				//entry* entryBuffer;
				
				for (int i = 0; i < vcb->numLba; i++)
				{
					printf("Bitmap Index %d: %c\n", i, *(bitMapBuf + i));
				}
				


	} else {
		printf("this is a new volume INIT STUFF\n");	

		//create volume control block and init values
		volumeEntry vcbTemp;
		vcbTemp.startOfVolume = '^';
		vcbTemp.numLba = lbaCount;
		//vcbTemp.freeInfoLocation = #;
		//vcbTemp.rootDirLocation = #;
		strcpy(vcbTemp.volumeName, fileName);
		vcbTemp.volumeID = 0;
		vcbTemp.lbaSize = blockSize;

		vcb = &vcbTemp;
		LBAwrite(vcb, 1, trackPosition);
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



		//write bitMap array to volume
		for(int i = 0; i < lbaCount; i++) {
			if(i < (trackPosition + (((sizeof(entry) * lbaCount) / blockSize) + 1)))
			{
				bitMap[i] = '1'; //all the blocks used for metadata are in use
			}
			else 
			{
				bitMap[i] = '0'; //remainder are free
			}
		}
		
		//write an empty entry array of the max possible entry size to the new volume
		entry* entryListBuf = entries;
		int entryListBlksWritten = LBAwrite(entryListBuf, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
		trackPosition += entryListBlksWritten;



		//TODO test if the volume contains the write values for bitmap and entries array
	}
	


	return trackPosition;
	
}
