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
#include "logicalFS.h"

//command prompt functions
void listDir(int currentDirIndex, entry* entries, int size);
int changeDir(char* args[], int currentDirIndex, entry* entryList, int size);
void makeDir(char* args[]);
void rmDir(char* args[]);
void rmFile(char* args[]);
void addFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, uint64_t blockSize, int lbaCount);
int cpFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int size);
void mvFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, uint64_t blockSize, int size);
void copyNormaltoCurrent(char* args[]);
void copyCurrenttoNormal(char* args[]);

int main(int argc, char* argv[]) {
	uint64_t vSize = 500000;
	uint64_t* volSize = &vSize;
	char* fileName = "OurVolume";
	uint64_t bSize = 512;
	uint64_t* blockSize = &bSize;
	uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry)) * (bSize - 1);
	uint64_t rootDirBlocks = (numBytes / bSize);
	uint64_t numDirEntries = (rootDirBlocks * bSize) / sizeof(entry);
	uint64_t lbaCount = (*volSize / *blockSize) - 1;
	
	int status = startPartitionSystem(fileName, volSize, blockSize);

	//metadata
	volumeEntry* vcb = malloc(*blockSize);
	entry* entryList = malloc(rootDirBlocks * bSize);// = malloc((((sizeof(entry) * lbaCount) / bSize) + 1) * bSize );
	char* bitMapBuf = malloc((((sizeof(char) * lbaCount) / bSize) + 1) * bSize);
	init(vcb, bitMapBuf, entryList, *volSize, *blockSize, lbaCount, fileName);
	init(vcb, bitMapBuf, entryList, *volSize, *blockSize, lbaCount, fileName);
	int currentDirIndex = 0;

	// printf("The number of blocks used for metadata is %d blocks out of %ld blocks.\n", numMetadataBlocks, lbaCount);

	// // Testing to see if vcb works here
	printf("VCB Start of Volume: %c\n", vcb->startOfVolume);
	printf("VCB Num LBA:		 %d\n", vcb->numLba);
	printf("VCB Volume Name:	 %s\n", vcb->volumeName);
	printf("VCB Volume ID:		 %d\n", vcb->volumeID);
	printf("VCB LBA Size:		 %d\n", vcb->lbaSize);
	
	
	// for (int i = 0; i < vcb->numLba; i++) {
	// 	printf("Main Bitmap Index %d: %c\n", i, *(bitMapBuf + i));
	// }

	for (int i = 0; i < numDirEntries; i++){//numDirEntries; i++) {
		if((entryList + i)->id != 0)
			printf("Main Entry Index %d: %s\n", i, (entryList + i)->name);
	}
	

	// Testing writeVolume function by writing 3 files
	// char testString[128] = "dfjaslg";
	// char testFileName[128] = "Yard";
	// char testFileName2[128] = "Blanket";
	// char testFileName3[128] = "House";
	// char * testBuffer = testString;
	//writeVolume(testBuffer, testFileName, 256, 0, 1, entryList, bitMapBuf, *blockSize, lbaCount);
	//writeVolume(testBuffer, testFileName2, 256, 0, 1, entryList, bitMapBuf, *blockSize, lbaCount);
	//writeVolume(testBuffer, testFileName3, 256, 1, 1, entryList, bitMapBuf, *blockSize, lbaCount);
	//deleteVolume(20, 256, entryList, bitMapBuf, *volSize, *blockSize, lbaCount);


	// for (int i = 0; i < vcb->numLba; i++) {
	// 	printf("Main Entry Index %d: %s\n", i, (entryList + i)->name);
	// }
	char line[1024];
	char* args[500];

  	while (1) {
  		printf("\nThe current directory is: %s\n", (entryList + currentDirIndex) -> name);
  	
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
			addFile(args, currentDirIndex, entryList, bitMapBuf, bSize, lbaCount);
		}
		else if(strcmp(args[0],"cp") == 0) {
			cpFile(args, currentDirIndex, entryList, bitMapBuf, lbaCount);
		}
		else if(strcmp(args[0],"mv") == 0) {
			mvFile(args, currentDirIndex, entryList, bitMapBuf, bSize, lbaCount);
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

void listDir(int currentDirIndex, entry* entryList, int size) {
	int entryCount = 0;
	for(int i = 0; i < size; i++) {
		if((entryList + i) -> parent == currentDirIndex && (entryList + i)->bitMap != ENTRYFLAG_UNUSED) {
			printf("%s ", (entryList + i) -> name);
			entryCount++;
			if(entryCount % 4 == 0)
				printf("\n");
		}
	}	
	printf("\n");
}
int changeDir(char* args[], int currentDirIndex, entry* entryList, int size) {
	int result = -1;
	int prevResult =-1;
	//Note that "cd " returns args[1] = null 
	if(args[1] == NULL){

		args[1] = "Root";
	}

	for(int i = 0; i < size; i++) {
		

		if((strcmp(args[1], (entryList + i) -> name) == 0) && ((entryList + i) -> bitMap == ENTRYFLAG_DIR)) {		
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
		// 	if(strcmp(args[1],"..") == 0){
		// 		result = prevResult;	
		// 	}
	} 
	//printf("result: %d\n",result);
	//printf("prevResult: %d\n",prevResult);
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

void addFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, uint64_t blockSize, int size) {
	if(args[1] == NULL){
		printf("Specify name of file to add, followed by the text input for your file.");
	}
	else if(args[2] == NULL){
		printf("Specify name of file to add, followed by the text input for your file.");
	}

	uint64_t inputSize = strlen(args[2]) + 1;
	char input[inputSize];
	strcpy(input, args[2]);
	char* buf = input;

	printf("Input String %s size is %ld bytes\n", input, strlen(args[2]) + 1);
    //writeToVolume(buf, args[1], inputSize, currentDirIndex, ENTRYFLAG_FILE, entryList, bitMap, blockSize, lbaCount);
}

int cpFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, int lbaCount) {
	int fileInCurrentDir = 0;
	int dirExists = 0;
	int result = -2;

	for(int i = 0; i < lbaCount; i++) {
		if(((entryList + i) -> bitMap == ENTRYFLAG_DIR) && ((entryList + i) -> parent == (entryList + currentDirIndex) -> index) && strcmp((entryList + i) -> name, args[1]) == 0) {
			printf("You can only copy files with this command, not directories.\n");
			return result;
		}
		else if(((entryList + i) -> parent == (entryList + currentDirIndex) -> index) && strcmp((entryList + i) -> name, args[1]) == 0) {
			fileInCurrentDir = 1;
			result = i;
			break;
		}
	}
	if(fileInCurrentDir == 0) {
		printf("There is no file in current directory with that name.\n");
	}

	for(int i = 0; i < lbaCount; i++) {
		//check for directory specified
		if((strcmp(args[2], (entryList + i) -> name) == 0) && ((entryList + i) -> bitMap == ENTRYFLAG_DIR)) {
			dirExists = 1;
			for(int j = 0; j < lbaCount; j++) {
				if(((entryList + j) -> parent == (entryList + i) -> index) && strcmp((entryList + j) -> name, args[1]) == 0){
					printf("There's already a file in destination directory with the same name.\n");
					result = -2;
					break;
				}
			}
		}
	}
	if(dirExists == 0) {
		result = -2;
		printf("The destination directory specified does not exist.\n");
	}

	else if(result != -2) {
		printf("TEST: file can be copied.\n");
		//readFromVolume into buffer
		//calculate input size into var
			//writeToVolume(buffer from readFromVolume, args[1], inputSize var, currentDirIndex, ENTRYFLAG_FILE, entryList, bitMap, blockSize, lbaCount);
	}

	return result; //this is the index of the orginal file that move will used to delete the old file
} 

void mvFile(char* args[], int currentDirIndex, entry* entryList, char* bitMap, uint64_t blockSize, int lbaCount){
	int indexOfFileToDel = cpFile(args, currentDirIndex, entryList, bitMap, lbaCount);
	//delete entrylist with index indexOfFileToDel
	if(indexOfFileToDel != -2) {
		printf("TEST: Valid mv cmd entered. Original file at entrylist index %d will be deleted\n", indexOfFileToDel);
		//calculate file index
		//calculate file size
	  	//deleteFromVolume(fileIndex value,fileSize var entryList, bitMap, blockSize, lbaCount)
	}
}

void copyNormaltoCurrent(char* args[]){
		//TODO
}

void copyCurrenttoNormal(char* args[]){
		//TODO
}