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
void addFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size);
int cpFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size);
void mvFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size);
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
  		printf("\nThe current directory is: %s\n", (entryList +currentDirIndex) -> name);


	    if(argv[1] == NULL)
	        argv[1] = ">";
	    printf("%s ",argv[1]);

	    // reading user line
	    if(fgets(line, sizeof(line), stdin) == NULL){
	         exit(EXIT_SUCCESS);
	    }

        line[strlen(line) - 1] = '\0';  // removes '\n' from fgets

		//error message for empty line
        if(strcmp(line,"")== 0 ){
            printf("Error: Empty line. Try again. \n");
            continue;
        }
      	if(strcmp(line," ")== 0 ){
           	printf("command not found.\n");
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
		else if(strcmp(args[0],"addFile") == 0) {
			//addFile(args, currentDirIndex, entryList, bitMapBuf lbaCount);
		}
		else if(strcmp(args[0],"cp") == 0) {
			cpFile(args, currentDirIndex, entryList, bitMapBuf, lbaCount);
		}
		else if(strcmp(args[0],"mv") == 0) {
			mvFile(args, currentDirIndex, entryList, bitMapBuf, lbaCount);
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



	 //test
	 char* buftest = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus non";

	 LBAwrite(buftest, 2, 100);
	 strcpy(entries[30].name, "foo");
	 entries[30].createTime = time(NULL);
	 entries[30].lastModified = time(NULL);
	 entries[30].bitMap = 1;
	 entries[30].location = 200;
	 entries[30].parent = 2;
	 entries[30].index = 11;



		entryList = entries;
		int entryListBlksWritten = LBAwrite(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
		trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	}





	return trackPosition;	
}

void listDir(int currentDirIndex, entry* entryList, int size) {
	int entryCount = 0;
	for(int i = 0; i < size; i++) {
		if((entryList + i) -> parent == currentDirIndex) {
			printf("%s ", (entryList + i) -> name);
			entryCount++;
			if(entryCount % 4 == 0)
				printf("\n");
		}
	}	
	printf("\n");
}
int prevResult =-1;
int changeDir(char* args[], int currentDirIndex, entry* entryList, int size) {
	int result = -1;
	// int prevResult =-1;
	//Note that "cd " returns args[1] = null 
	if(args[1] == NULL){

		args[1] = "Root";
	}

	for(int i = 0; i < size; i++) {
		

		if((strcmp(args[1], (entryList + i) -> name) == 0) && ((entryList + i) -> bitMap == 0)) {		
			//Check if the entered directory is the child of the current directory
			if((entryList + i) -> parent == (entryList + currentDirIndex) -> index) {
				result = i;
				if(i == -1) {
					prevResult = -1;
				}
				else{prevResult = i-1;}
			}
			//Check if the entered directory is the parent of the current directory
			if((entryList + i) -> index == (entryList + currentDirIndex) -> parent) {
				result = i;
				if(i == -1 ) {
					prevResult = -1;
				}
				else{prevResult = i-1;}
			}

		}
			if(strcmp(args[1],"..") == 0){
				// printf("result in ..: %d\n",result);
				// if(i == -1 ) 
				// 	prevResult = -1;
				// if(result == prevResult){

				// 	prevResult= prevResult - 1;
				// }
				result = prevResult;
				
				// 	if(prevResult == -1 ) 
				// 		prevResult = -1;

			}
	} 
	printf("result: %d\n",result);
	printf("prevResult: %d\n",prevResult);
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

void addFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size) {
		//TODO
}

int cpFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size) {
	int fileInCurrentDir = 0;
	int dirExists = 0;
	int result = -2;

	for(int i = 0; i < size; i++) {
		if(((entryList + i) -> parent == (entryList + currentDirIndex) -> index) && strcmp((entryList + i) -> name, args[1]) == 0) {
			fileInCurrentDir = 1;
			result = i;
			break;
		}
	}
	if(fileInCurrentDir == 0) {
		printf("There is no file in current directory with that name.\n");
	}


	for(int i = 0; i < size; i++) {
		//check for directory specified
		if((strcmp(args[2], (entryList + i) -> name) == 0) && ((entryList + i) -> bitMap == 0)) {
			dirExists = 1;
			for(int j = 0; j < size; j++) {
				if(((entryList + j) -> parent == (entryList + i) -> index) && strcmp((entryList + j) -> name, args[1]) == 0){
					printf("There's already a file in destination directory with the same name.\n");
					result = -2;
					break;
				}

	 			//check bitmap for an unused spot
	 			//check size of args1 file and get # blocks to assign for write
		     	//write metadata for new entry with name args2 and #blocks, etc
	 			//write to LBA a new entry 
			}

		}
	}

	if(dirExists == 0)
		printf("The destination directory specified does not exist.");

	return result; //this is the index of the orginal file that move will used to delete the old file
} 

void mvFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size){
	int indexOfFileToDel = cpFile(args, currentDirIndex, entryList, bitMap, size);
	//delete entrylist with index indexOfFileToDel
	printf("original file at entrylist index %d will be deleted\n", indexOfFileToDel);
}

void copyNormaltoCurrent(char* args[]){
		//TODO
}

void copyCurrenttoNormal(char* args[]){
		//TODO
}