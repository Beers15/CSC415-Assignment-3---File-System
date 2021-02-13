/************************************************************** 
* Class:  CSC-415-0# Spring 2020 
* Group Name: Kernel Sanders
* Name: Alexander Beers
* Student ID: 917308410
* Name: Alicia Ramirez
* Student ID: 917379715
* Name: Michael Canson
* Student ID: 920601003
* Name: Amron Berhanu
* Student ID: 916320644
* Project: Assignment 3 – File System 
* 
* File: logicalFS.h
* 
* Description: Contains the necessary macros, libraries, and function prototypes
*			   for our logical file system.
* **************************************************************/
#ifndef LOGICALFS_H
#define LOGICALFS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"
#include "volumeEntry.h"
#include "fsLow.h"
#include <fcntl.h>
#include <unistd.h>

#define AVGDIRENTRIES 100
#define ENTRYFLAG_DIR 0
#define ENTRYFLAG_FILE 1
#define ENTRYFLAG_UNUSED 2
#define BLOCK_SIZE 512
#define VOLUME_SIZE 10000000
#define LBA_COUNT VOLUME_SIZE / BLOCK_SIZE

void initRoot(uint64_t position, entry entries[], entry* entryList, uint64_t rootDirBlocks, uint64_t numDirEntries);
void init(volumeEntry* vcb, char* bitMapBuf, entry entries[], entry* entryList, char* fileName, uint64_t rootDirBlocks, uint64_t numDirEntries);
void* readFromVolume(char fileName[], entry* entryList, uint64_t numDirEntries, int currentDirIndex);
int writeToVolume(void* buffer, char fileName[], uint64_t fileSize, int currentDirIndex, uint16_t type, entry* entryList, char* bitMap);
int writeDirectoryToVolume(char dirName[], int currentDirIndex, uint16_t type, entry* entryList, char* bitMap);
int deleteFromVolume(int fileIndex, entry* entryList, char* bitMap);
int deleteDirectoryFromVolume(int directoryIndex, entry* entryList, char* bitMap);

#endif