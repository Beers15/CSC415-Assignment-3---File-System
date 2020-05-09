#include "logicalFS.h"

//the params here will have to be assigned to whatever already exists on the volume if it isn't a new volume
void init(volumeEntry* vcb, char* bitMapBuf, entry entries[], entry* entryList, char* fileName, uint64_t rootDirBlocks, uint64_t numDirEntries) {
	//int blocksNeeded = #; //calculate after figturing out init block size for all metadata
	uint64_t trackPosition = 0; 
	char bitMap[LBA_COUNT];
	uint64_t statusForRead = LBAread(vcb, 1, 0);

	if(vcb->startOfVolume == '!') {
			 

		trackPosition += 1; //vcb is always < 1 full block
		printf("Reading in metadata to program...\n");
		uint64_t statusForRead2 = LBAread(bitMapBuf, (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1), trackPosition);
		trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);
		uint64_t statusForRead3 = LBAread(entryList, rootDirBlocks, trackPosition);
		trackPosition += rootDirBlocks;
	} else {
		printf("New volume, intializing metadata...\n");

		//create volume control block and init values
		volumeEntry vcbTemp;
		vcbTemp.startOfVolume = '!';
		vcbTemp.numLba = LBA_COUNT;
		strcpy(vcbTemp.volumeName, fileName);
		vcbTemp.volumeID = 0;
		vcbTemp.lbaSize = BLOCK_SIZE;

		vcb = &vcbTemp;
		LBAwrite(vcb, 1, trackPosition);
		trackPosition += 1; //vcb is always < 1 full block
		
		//write bitMap array to volume
		for(int i = 0; i < LBA_COUNT; i++) {
			if(i < (trackPosition + (((AVGDIRENTRIES * sizeof(entry)) * (BLOCK_SIZE - 1))) / BLOCK_SIZE)) {
				bitMap[i] = '1'; //all the blocks used for metadata are in use
			}
			else {
				bitMap[i] = '0'; //remainder are free
			}
		}
		
		bitMapBuf = bitMap;
		int bitMapBlksWritten = LBAwrite(bitMapBuf, (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1), trackPosition);
		trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);
		initRoot(trackPosition, entries, entryList, rootDirBlocks, numDirEntries);
	}
}

void initRoot(uint64_t position, entry entries[], entry* rootBuf, uint64_t rootDirBlocks, uint64_t numDirEntries) {
		//entryList is the rootBuffer
		printf("InitRoot\nPosition: %lu\nRootDirBlocks: %lu\nnumDirEntries: %lu\n", position, rootDirBlocks, numDirEntries);

		//init max potential entries in root dir
		for(int i = 0; i < numDirEntries; i++) {
			strcpy(entries[i].name, "-1");
			entries[i].bitMap = ENTRYFLAG_UNUSED;
			entries[i].id = 0;
			entries[i].parent = -2; //filler value to show this entry is unassigned
		}

		//the root dir 
		strcpy(entries[0].name, "Root");
		entries[0].location = position;
		entries[0].index = 0;
		entries[0].id = -1; //to signify root dir
		entries[0].bitMap = ENTRYFLAG_DIR;
		entries[0].createTime = time(NULL);
		entries[0].lastModified = time(NULL);
		entries[0].parent = -1; //no parent for root
		entries[0].count = numDirEntries; 

		//test
		entries[2].id = 13111; //to signify root dir
		entries[2].bitMap = ENTRYFLAG_DIR;
		strcpy(entries[2].name, "Test");
		//entries[2].location = position;
		entries[2].index = 2;
		entries[2].createTime = time(NULL);
		entries[2].lastModified = time(NULL);
		entries[2].parent = 0;
		entries[2].count = 1; 

		entries[3].id = 111111; //to signify root dir
		entries[3].bitMap = ENTRYFLAG_DIR;
		strcpy(entries[3].name, "Test2");
		//entries[3].location = position;
		entries[3].index = 3;
		entries[3].createTime = time(NULL);
		entries[3].lastModified = time(NULL);
		entries[3].parent = 2;
		entries[3].count = 1; 

		entries[4].id = 3423423; //to signify root dir
		entries[4].bitMap = ENTRYFLAG_FILE;
		strcpy(entries[4].name, "alphatest");
		//entries[4].location = position;
		entries[4].index = 4;
		entries[4].createTime = time(NULL);
		entries[4].lastModified = time(NULL);
		entries[4].parent = 2;
		entries[4].location = 200;
		entries[4].count = 1; 

		char* buf = "ENTRY 4 filename TEST: THIS IS A TEST 11111111111111111111111111111111111111111111111111111111222222222222222222222222222222222222222222222222222222222222233333333333333333333333333333333";
		LBAwrite(buf, 1, 200);


		entries[5].id = 3422223; //to signify root dir
		entries[5].bitMap = ENTRYFLAG_FILE;
		strcpy(entries[5].name, "testfile");
		entries[5].location = 300;
		entries[5].index = 5;
		entries[5].createTime = time(NULL);
		entries[5].lastModified = time(NULL);
		entries[5].parent = 0;
		entries[5].count = 1; 

		char* buf2 = "####################@@@@@@@@@@@@@@@@@@@((((((((((((((((()))))))))))))))))))(((((((((((((((((((((((($$$$$$$$$$$$$$$$$$$";
		LBAwrite(buf2, 1, 300);


		rootBuf = entries;
		LBAwrite(rootBuf, rootDirBlocks, position);
}

// Reads file contents from disk into a buffer that is returned to the caller
void* readFromVolume(char fileName[], entry* entryList, uint64_t numDirEntries, int currentDirIndex) {
	uint64_t filePosition;
	uint64_t fileBlockCount;
	void* buffer; // generic buffer
	int found = 0;

	// Iterate entryList to find filename
	// Update entryList with new entry
	for(int i = 1; i < numDirEntries; i++) {
		// Check if the filename exists in the current directory
		if(strcmp((entryList + i)->name, fileName) == 0 && ((entryList + i)->parent == currentDirIndex))
		{
			found = 1;
			filePosition = (entryList + i)->location;
			fileBlockCount = (entryList + i)->count;

			buffer = malloc(BLOCK_SIZE * fileBlockCount);

			// LBA read file into buffer
			LBAread(buffer, fileBlockCount, filePosition);

			break;
		}
	}

	// Case where filename doesnt exist in the current directory
	if(found == 0) {
		printf("The file specified is not in the current directory.\n");
		buffer = NULL;
		// Or
		//return -1;
	}
	
	return buffer;
}

// Writes the given buffer into the next available space in the volume as long as there's enough memory
int writeToVolume(void* buffer, char fileName[], uint64_t fileSize, int currentDirIndex, uint16_t type, entry* entryList, char* bitMap)
{
	int freeBlockStart;
	int inFreeSection = 0;
	int freeBlockExtent;
	uint64_t fileBlockSize = ((fileSize / BLOCK_SIZE) + 1);

	//Loop through bitMap until we find a section that has enough consecutive blocks to write to.
	//Once found save into freeBlockStart(block to start writing at) and freeBlockExtent(For how many blocks to write)
	for(int i = 0; i < LBA_COUNT; i++)
	{
		if(*(bitMap + i) == '0' && inFreeSection == 0)
		{
			inFreeSection = 1;
			freeBlockStart = i;
			freeBlockExtent = 1;
		}
		else if(*(bitMap + i) == '0' && inFreeSection == 1)
		{
			freeBlockExtent++;
		}
		else if(*(bitMap + i) != '0')
		{
			freeBlockStart = -1;
			freeBlockExtent = 0;
			inFreeSection = 0;
		}
		if(freeBlockExtent == fileBlockSize)
		{
			break;
		}
	}
	
	// This error occurs if not enough consecutive amount of blocks.
	if(freeBlockExtent < fileBlockSize)
	{
		printf("ERROR: Not enough memory.\n");
		return -1;
	}
	
	// If there is an error writing the buffer
	if( LBAwrite(buffer, freeBlockExtent, freeBlockStart) <= 0 )
	{
		printf("ERROR: Could not write to memory.\n");
		return -1;
	}
	
	// Update bitMap
	for(int i = freeBlockStart; i < (freeBlockStart + freeBlockExtent); i++)
	{
		*(bitMap + i) = '1';
	}
	
	//To write bitmap and entries, trackPosition starts at 1 since we don't need to write vcb again
	int trackPosition = 1;
	int bitMapBlksWritten = LBAwrite(bitMap, (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1), 1);
	trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);

	uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry));
	uint64_t rootDirBlocks = ((numBytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE);
	uint64_t numDirEntries = (rootDirBlocks * BLOCK_SIZE) / sizeof(entry);
	

	int duplicate = 0;
	for(int i = 0; i < numDirEntries; i++) {
		if((strcmp((entryList + i)->name, fileName) == 0) && (entryList + i)->parent == currentDirIndex) {
			printf("\n%s %d\n", (entryList + i)->name,(entryList + i)->parent);
				printf("There is already an entry with the same name in the folder being written to. Please try again.\n");
				duplicate = 1;
			}
	}

	if(duplicate == 0) {
		// Update entryList with new entry
		for(int i = 0; i < numDirEntries; i++)
		{
			if((entryList + i)->parent == -2)
			{
				(entryList + i)->parent = currentDirIndex;
				strcpy((entryList + i)->name, fileName);
				(entryList + i)->count = freeBlockExtent;
				(entryList + i)->location = freeBlockStart;
				(entryList + i)->index = i;
				(entryList + i)->id = i;
				(entryList + i)->bitMap = type;
				(entryList + i)->createTime = time(NULL);
				(entryList + i)->lastModified = time(NULL);
				break;
			}
		}
		int entryListBlksWritten = LBAwrite(entryList, rootDirBlocks , trackPosition);
		trackPosition += rootDirBlocks;
	}	
	
	return 0;
	
}


// Writes the given buffer into the next available space in the volume as long as there's enough memory
int writeDirectoryToVolume(char dirName[], int currentDirIndex, uint16_t type, entry* entryList, char* bitMap)
{
	
	//To write bitmap and entries, trackPosition starts at 1 since we don't need to write vcb again
	int trackPosition = 1;
	trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);

	uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry));
	uint64_t rootDirBlocks = ((numBytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE);
	uint64_t numDirEntries = (rootDirBlocks * BLOCK_SIZE) / sizeof(entry);
	
	// Update entryList with new entry
	for(int i = 0; i < numDirEntries; i++)
	{
		if((entryList + i)->parent == -2)
		{
			(entryList + i)->parent = currentDirIndex;
			strcpy((entryList + i)->name, dirName);
			(entryList + i)->count = -2;
			(entryList + i)->location = -2;
			(entryList + i)->index = i;
			(entryList + i)->id = i;
			(entryList + i)->bitMap = type;
			(entryList + i)->createTime = time(NULL);
			(entryList + i)->lastModified = time(NULL);
			break;
		}
	}
	int entryListBlksWritten = LBAwrite(entryList, rootDirBlocks , trackPosition);
	trackPosition += rootDirBlocks;
	
	
	return 0;
	
}


// Deletes the file at the specified entryList index
int deleteFromVolume(int fileIndex, entry* entryList, char* bitMap)
{
	// Actually  erase data for protection
	void * blank = malloc(((entryList + fileIndex)->count) * BLOCK_SIZE);
	LBAwrite(blank, (entryList + fileIndex)->count, (entryList + fileIndex)->location);

	// Update bitMap by zeroing out what was once used space, now it can be written over by new files.
	for(int i = (entryList + fileIndex)->location; i < ((entryList + fileIndex)->location + (entryList + fileIndex)->count); i++)
	{
		*(bitMap + i) = '0';
	}
	
	//To write bitmap and entries, trackPosition starts at 1 since we don't need to write vcb again
	int trackPosition = 1;
	int bitMapBlksWritten = LBAwrite(bitMap, (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1), 1);
	trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);
	
	uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry));
	uint64_t rootDirBlocks = ((numBytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE);
	uint64_t numDirEntries = (rootDirBlocks * BLOCK_SIZE) / sizeof(entry);
	
	// Update entryList by nulling entry
	strcpy((entryList + fileIndex)->name, "");
	(entryList + fileIndex)->count = 0;
	(entryList + fileIndex)->location = 0;
	(entryList + fileIndex)->bitMap = ENTRYFLAG_UNUSED;
    (entryList + fileIndex)->id = 0;
	(entryList + fileIndex)->parent = -2;
	
	
	int entryListBlksWritten = LBAwrite(entryList, rootDirBlocks , trackPosition);
	trackPosition += rootDirBlocks;
	
	return 0;
}

// Deletes the directory at the specified entryList index
int deleteDirectoryFromVolume(int fileIndex, entry* entryList, char* bitMap)
{
	
	int trackPosition = 1;
	trackPosition += (((sizeof(char) * LBA_COUNT) / BLOCK_SIZE) + 1);
	
	uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry));
	uint64_t rootDirBlocks = ((numBytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE);
	uint64_t numDirEntries = (rootDirBlocks * BLOCK_SIZE) / sizeof(entry);
	
	// Update entryList by nulling entry
	strcpy((entryList + fileIndex)->name, "");
	(entryList + fileIndex)->count = 0;
	(entryList + fileIndex)->location = 0;
	(entryList + fileIndex)->bitMap = ENTRYFLAG_UNUSED;
    (entryList + fileIndex)->id = 0;
	(entryList + fileIndex)->parent = -2;
	
	
	int entryListBlksWritten = LBAwrite(entryList, rootDirBlocks , trackPosition);
	trackPosition += rootDirBlocks;
	
	return 0;
}