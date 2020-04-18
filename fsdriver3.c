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

void readVolume(char* volumeName);
void writeVolume();
//command prompt functions
void listDir(int currentDirIndex, entry* entries, int size);
int changeDir(char* args[], int currentDirIndex, entry* entryList, int size);
void makeDir(char* args[]);
void rmDir(char* args[]);
void rmFile(char* args[]);
void addFile(char* args[]);
void cpFile(char* args[]);
void mvFile(char* args[]);
void copyNormaltoCurrent(char* args[]);
void copyCurrenttoNormal(char* args[]);



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
	int currentDirIndex = -1;

	// printf("The number of blocks used for metadata is %d blocks out of %ld blocks.\n", numMetadataBlocks, lbaCount);

	// // Testing to see if vcb works here
	// printf("VCB Start of Volume: %c\n", vcb->startOfVolume);
	// printf("VCB Num LBA:		 %d\n", vcb->numLba);
	// printf("VCB Volume Name:	 %s\n", vcb->volumeName);
	// printf("VCB Volume ID:		 %d\n", vcb->volumeID);
	// printf("VCB LBA Size:		 %d\n", vcb->lbaSize);
	
	
	// for (int i = 0; i < vcb->numLba; i++) {
	// 	printf("Main Bitmap Index %d: %c\n", i, *(bitMapBuf + i));
	// }

	// for (int i = 0; i < 20; i++) {
	// 	printf("Main Entry Index %d: %s\n", i, (entryList + i)->name);
	// }
	
	//have program start it root directory
	for(int i = 0; i < vcb->numLba; i++) {
		if((entryList + i)->parent == -1) {
			currentDirIndex = i;
			printf("Root directory is at index %d\n", currentDirIndex);
			break;
		}
	}

	char line[1024];
	char* args[500];

  	while (1) {
  		printf("\nindex of the current directory in entries storage: %d\n", currentDirIndex);


	    if(argv[1] == NULL)
	        argv[1] = ">";
	    printf("%s ",argv[1]);

	    // reading user line
	    if(fgets(line, sizeof(line), stdin) == NULL){
	         exit(EXIT_SUCCESS);
	    }

        line[strlen(line) - 1] = '\0';  // removes '\n' from fgets

		//error message for empty line
        if(!strcmp(line,"\n")){
            printf("Error: Empty line. Try again. \n");
            continue;
        }

        //when user decides to exit
        if (!strcmp(line, "exit")) {
           return 0;
        }

        int i = 0;
        args[0] = strtok(line, " ");
		
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
			// printf("args: %s\n",args[i]); //TEST
        }

		if(strcmp(args[0],"ls") == 0) {
			listDir(currentDirIndex, entryList, lbaCount); 
		}
		else if(strcmp(args[0],"cd") == 0) {
			int indexVal = changeDir(args, currentDirIndex, entryList, lbaCount);
			if(indexVal != -1)
				currentDirIndex = indexVal;
		}
		else if(strcmp(args[0],"mkdir") == 0) {
			makeDir(args);
		}
		// else if(strcmp(args[0],"rmdir") == 0) {
		// 	rmDir(args);
		// }
		else if(strcmp(args[0],"rm") == 0) {
			rmFile(args);
		}
			else if(strcmp(args[0],"cat") == 0) {
			addFile(args);
		}
		else if(strcmp(args[0],"cp") == 0) {
			cpFile(args);
		}
		else if(strcmp(args[0],"mv") == 0) {
			mvFile(args);
		}
		else if(strcmp(args[0],"cpn") == 0) {
			copyNormaltoCurrent(args);
		}
		else if(strcmp(args[0],"cpc") == 0) {
			copyCurrenttoNormal(args);
		}
		else{
			//TODO not sure if theres some type of error message to call
			printf("command not found.\n");
		}
	}

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
		printf("Reading in metadata to program...\n");
		uint64_t statusForRead2 = LBAread(bitMapBuf, (((sizeof(char) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
		uint64_t statusForRead3 = LBAread(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	} else {
		printf("New volume, intializing metadata...\n");

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
		
		//write an empty entry array of the max possible entry size to the new volume

		//create metadata for root directory
		for(int i = 0; i < lbaCount; i++) {
			if(i == 0) {
				strcpy(entries[i].name, "Root");
				entries[i].size = 1;
				entries[i].location = trackPosition; //Lba block location
				entries[i].index = i;
				entries[i].createTime = time(NULL);
				entries[i].lastModified = time(NULL);
				entries[i].bitMap = 0; //dir
				entries[i].parent = -1; // filler value to show that this is value without parent
				//entries[i].permissions = 
				entries[i].count = 1;
				//TODO Call write file/directory to block @ trackPosition's value
			}
					//test entries for cd rmFile
					else if(i == 1) {
						strcpy(entries[i].name, "TestDir");
						entries[i].createTime = time(NULL);
						entries[i].lastModified = time(NULL);
						entries[i].bitMap = 0;
						entries[i].location = 100; //some bogus test value for lba containing this dir
						entries[i].index = i;
						entries[i].parent = 0; //parent is root dir
						entries[i].count = 1;
					}
					else if(i == 2) {
						strcpy(entries[i].name, "Test2Dir");
						entries[i].createTime = time(NULL);
						entries[i].lastModified = time(NULL);
						entries[i].bitMap = 0;
						entries[i].location = 101; //some bogus test value for lba containing this dir
						entries[i].index = i;
						entries[i].parent = 1; //parent is Test
						entries[i].count = 1;
					}
					else if(i > 3 && i < 10) {
						strcpy(entries[i].name, "SomeFile");
						entries[i].createTime = time(NULL);
						entries[i].lastModified = time(NULL);
						entries[i].bitMap = 1;
						entries[i].location = (100 + i); //some bogus test value for lba containing this dir
						entries[i].index = i;
						entries[i].parent = 1; //parent is Test
						entries[i].count = 1;
					}
			else {
				strcpy(entries[i].name, "-1");
				entries[i].parent = -2; //filler value to show this entry is unassigned
			}
		}

		entryList = entries;
		int entryListBlksWritten = LBAwrite(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
		trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	}
	return trackPosition;	
}

// void readVolume(char* volumeName) {
// 	FILE* volume = fopen(volumeName, "r");
// 	if(f == NULL) {
//     	perror("Could not read in the FS that was specified.");
//     	return 1;
// 	}
// 	else {
// 		//parse the file into vars used for init after assigning the VCB startOfVolume val to = '^'
// 		//call init
// 	}
// }

void listDir(int currentDirIndex, entry* entries, int size) {
	int entryCount = 0;
	for(int i = 0; i < size; i++) {
		if((entries + i) -> parent == currentDirIndex) {
			printf("%s ", (entries + i) -> name);
			entryCount++;
			if(entryCount % 4 == 0)
				printf("\n");
		}
	}	
	printf("\n");
}

int changeDir(char* args[], int currentDirIndex, entry* entryList, int size) {
	int result = -1;
	for(int i = 0; i < size; i++) {
		if((strcmp(args[1], (entryList + i) -> name) == 0) && ((entryList + i) -> bitMap == 0)) {
			//Check if the entered directory is the child of the current directory
			if((entryList + i) -> parent == (entryList + currentDirIndex) -> index) {
				result = i;
				printf("The parent of the directory matching argv1 is the current dir\n");
			}
			//Check if the entered directory is the parent of the current directory
			if((entryList + i) -> index == (entryList + currentDirIndex) -> parent) {
				result = i;
				printf("the entered directory is the parent of the current directory\n");
			}
		}
	} 
	return result;
}

void makeDir(char* args[]) {
		//TODO
}

void rmDir(char* args[]){
		//TODO
}

void rmFile(char* args[]){
		//TODO
}

void addFile(char* args[]){
		//TODO
}

void cpFile(char* args[]){
		//TODO
}

void mvFile(char* args[]){
		//TODO
}

void copyNormaltoCurrent(char* args[]){
		//TODO
}

void copyCurrenttoNormal(char* args[]){
		//TODO
}