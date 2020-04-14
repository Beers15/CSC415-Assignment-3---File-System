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
#include <sys/types.h>
#include "fsLow.h"

struct entry {
	char name[128];
	unsigned long size;
	int address; 
	id_t id;
	time_t createTime;
	time_t lastModified;
	uint16_t bitMap;
	int parent;
	uint32_t permissions;
};

typedef struct VolumeEntry { 
    int numLba;
    int freeInfoLocations[3]; //for '3' copies of free info locations in volume
    int rootdirLocation; //location of root directory
    char volumeName[128];
    unsigned int volumeID;
    unsigned int lbaSize;// = 512;
    unsigned int clusterSize;// = 8;
} Ventry;

struct freeChunk {
    char sig1[8];
    int next; //index of next chunk in LBA
    int prev; //index of prev chunk in LBA
    long count;
    char sig2[8];
};

//opposite of free space
struct extents {
    char sig1[8];
    int fileID; //can be directory as well
    int next; //next chunk of file
    int prev; //prev chunk of file
    long count;
    char sig2[8];
};

int main(int argc, char* argv[]) {

}