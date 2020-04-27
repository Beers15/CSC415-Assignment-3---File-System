#ifndef LOGICALFS_H
#define LOGICALFS_H

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"
#include "volumeEntry.h"
#include "fsLow.h"

#define AVGDIRENTRIES 50
#define ENTRYFLAG_DIR 0
#define ENTRYFLAG_FILE 1
#define ENTRYFLAG_UNUSED 2

void initRoot(uint64_t position, uint64_t blockSize, entry* entryList);
void init(volumeEntry* vcb, char* bitMapBuf, entry* entryList, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName);
void readFromVolume(char* volumeName);
int writeToVolume(void* buffer, char fileName[], uint64_t fileSize, int currentDirIndex, uint16_t type, entry* entryList, char* bitMap, uint64_t blockSize, uint64_t lbaCount);
int deleteFromVolume(int fileIndex, uint64_t fileSize, entry* entryList, char* bitMap, uint64_t blockSize, uint64_t lbaCount);

#endif