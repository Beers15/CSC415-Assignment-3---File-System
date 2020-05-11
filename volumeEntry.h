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
* File: volumeEntry.h
* 
* Description: Contains struct definition used to represent the metadata
*			   of the Volume Control Block within our File System.
* **************************************************************/
typedef struct VolumeEntry { 
    char startOfVolume;
    int numLba;
    int rootdirLocation; //location of root directory
    char volumeName[128];
    unsigned int volumeID;
    unsigned int lbaSize;
} volumeEntry;