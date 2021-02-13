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
* File: entry.h
* 
* Description: Contains struct definition used to represent the metadata
*			   of every entry within our File System aside from the VCB.
* **************************************************************/
#include <time.h>
#include <sys/types.h>// for id_t
#include <stdint.h>

typedef struct entry {
	char name[128];
	int location; //block number in LBA
	int index;
	id_t id;
	uint32_t bitMap; //to mark entry as file, dir, or unused entry via flags
	time_t createTime;
	time_t lastModified;
	int parent; //parent entryList index
	int count; //number of logical blocks
} entry;