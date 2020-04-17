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

int init(volumeEntry* vcb, char bitMap[], char* bitMapBuf, entry entries[], entry* entryList, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName);

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
	entry* entryList = malloc((((sizeof(entry) * lbaCount) / bSize) + 1) * bSize );
	char bitMap[lbaCount];
	entry entries[lbaCount];
	char* bitMapBuf = malloc((((sizeof(char) * lbaCount) / bSize) + 1) * bSize);
	int numMetadataBlocks = init(vcb, bitMap, bitMapBuf, entries, entryList, *volSize, *blockSize, lbaCount, fileName);
	init(vcb, bitMap, bitMapBuf, entries, entryList, *volSize, *blockSize, lbaCount, fileName);
	
	printf("The number of blocks used for metadata is %d blocks out of %ld blocks.\n", numMetadataBlocks, lbaCount);

	// Testing to see if vcb works here
	printf("VCB Start of Volume: %c\n", vcb->startOfVolume);
	printf("VCB Num LBA:		 %d\n", vcb->numLba);
	printf("VCB Volume Name:	 %s\n", vcb->volumeName);
	printf("VCB Volume ID:		 %d\n", vcb->volumeID);
	printf("VCB LBA Size:		 %d\n", vcb->lbaSize);
	
	
	for (int i = 0; i < vcb->numLba; i++) {
		printf("Main Bitmap Index %d: %c\n", i, *(bitMapBuf + i));
	}
	
	

	for (int i = 0; i < vcb->numLba; i++) {
		printf("Main Entry Index %d: %s\n", i, (entryList + i)->name);
	}
	

	//after init works main loop would go here prompting for commands
	
	closePartitionSystem();

	return 0;
}

//the params here will have to be assigned to whatever already exists on the volume if it isn't a new volume
int init(volumeEntry* vcb, char bitMap[], char* bitMapBuf, entry entries[], entry* entryList, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName) {
	//int blocksNeeded = #; //calculate after figturing out init block size for all metadata
	int trackPosition = 0; 
	uint64_t statusForRead = LBAread(vcb, 1, 0);

	if(vcb->startOfVolume == '^') {
		trackPosition += 1; //vcb is always < 1 full block
		printf("existing volume, read in metadata to program\n");
		uint64_t statusForRead2 = LBAread(bitMapBuf, (((sizeof(char) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
		uint64_t statusForRead3 = LBAread(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
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
		

		//write bitMap array to volume
		for(int i = 0; i < lbaCount; i++) {
			if(i < (trackPosition + (((sizeof(entry) * lbaCount) / blockSize) + 1))) {
				bitMap[i] = '1'; //all the blocks used for metadata are in use
			}
			else {
				bitMap[i] = '0'; //remainder are free
			}
		}
		
		bitMapBuf = bitMap;
		int bitMapBlksWritten = LBAwrite(bitMapBuf, (((sizeof(char) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
		
		for (int i = 0; i < lbaCount; i++) {
			strcpy(entries[i].name, "-1");
		}
		
		//write an empty entry array of the max possible entry size to the new volume
		entryList = entries;
		printf("%s\n", (entryList + 3)->name);
		int entryListBlksWritten = LBAwrite(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
		//trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	}
	return trackPosition;	
}