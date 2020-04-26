#include "logicalFS.h"

//the params here will have to be assigned to whatever already exists on the volume if it isn't a new volume
void init(volumeEntry* vcb, char* bitMapBuf, entry* entryList, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount, char* fileName) {
	//int blocksNeeded = #; //calculate after figturing out init block size for all metadata
	int trackPosition = 0; 
	char bitMap[lbaCount];
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
			if(i < (trackPosition + (((AVGDIRENTRIES * sizeof(entry)) * (blockSize - 1))) / blockSize)) {
				bitMap[i] = '1'; //all the blocks used for metadata are in use
			}
			else {
				bitMap[i] = '0'; //remainder are free
			}
		}
		
		bitMapBuf = bitMap;
		int bitMapBlksWritten = LBAwrite(bitMapBuf, (((sizeof(char) * lbaCount) / blockSize) + 1), trackPosition);
		trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
	printf("IN INIT: %s", (entryList + 0)->name);
		initRoot(trackPosition, blockSize, entryList);
	}
}

void initRoot(uint64_t position, uint64_t blockSize, entry* rootBuf) {
		//entryList is the rootBuffer
		uint64_t numBytes = (AVGDIRENTRIES * sizeof(entry)) * (blockSize - 1);
		uint64_t rootDirBlocks = (numBytes / blockSize);
		uint64_t numDirEntries = (rootDirBlocks * blockSize) / sizeof(entry);

		entry entries[numDirEntries];

		//init max potential entries in root dir
		for(int i = 0; i < numDirEntries; i++) {
			entries[i].id = 0;
			entries[i].bitMap = ENTRYFLAG_UNUSED;
			strcpy(entries[i].name, "");
			entries[i].location = 0;
			//rootBuf[i]->location = trackPosition;
		}

		//the root dir 
		entries[0].id = -1; //to signify root dir
		entries[0].bitMap = ENTRYFLAG_DIR;
		strcpy(entries[0].name, "Root");
		entries[0].location = position;
		entries[0].createTime = time(NULL);
		entries[0].lastModified = time(NULL);
		entries[0].parent = -1; //no parent for root
		entries[0].count = numDirEntries * blockSize; 

				//test
				entries[2].id = 13111; //to signify root dir
				entries[2].bitMap = ENTRYFLAG_DIR;
				strcpy(entries[2].name, "Test");
				entries[2].location = position;
				entries[2].createTime = time(NULL);
				entries[2].lastModified = time(NULL);
				entries[2].parent = 0;
				entries[2].count = 1; 

				entries[3].id = 111111; //to signify root dir
				entries[3].bitMap = ENTRYFLAG_DIR;
				strcpy(entries[3].name, "Test2");
				entries[3].location = position;
				entries[3].createTime = time(NULL);
				entries[3].lastModified = time(NULL);
				entries[3].parent = 2;
				entries[3].count = 1; 

		rootBuf = entries;
		LBAwrite(rootBuf, rootDirBlocks, position);
}

void readFromVolume(char* volumeName) {

}

// Writes the given buffer into the next available space in the volume as long as there's enough memory
int writeToVolume(void* buffer, char fileName[], uint64_t fileSize, int currentDirIndex, uint16_t bit, entry* entryList, char* bitMap, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount)
{
	int freeBlockStart;
	int inFreeSection = 0;
	int freeBlockExtent;
	uint64_t fileBlockSize = ((fileSize / blockSize) + 1);

	//Loop through bitMap until we find a section that has enough consecutive blocks to write to.
	//Once found save into freeBlockStart(block to start writing at) and freeBlockExtent(For how many blocks to write)
	for(int i = 0; i < lbaCount; i++)
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
	int bitMapBlksWritten = LBAwrite(bitMap, (((sizeof(char) * lbaCount) / blockSize) + 1), 1);
	trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
	
	// Update entryList with new entry
	for(int i = 0; i < lbaCount; i++)
	{
		if((entryList + i)->parent == -2)
		{
			(entryList + i)->parent = currentDirIndex;
			strcpy((entryList + i)->name, fileName);
			(entryList + i)->count = freeBlockExtent;
			(entryList + i)->location = freeBlockStart;
			(entryList + i)->index = i;
			//(entryList + i)->id =
			(entryList + i)->bitMap = bit;
			(entryList + i)->lastModified = time(NULL);
			break;
		}
	}
	int entryListBlksWritten = LBAwrite(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
	trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	
	return 0;
	
}

// Deletes the file at the specified entryList index
int deleteFromVolume(int fileIndex, uint64_t fileSize, entry* entryList, char* bitMap, uint64_t volumeSize, uint64_t blockSize, uint64_t lbaCount)
{
	uint64_t fileBlockSize = ((fileSize / blockSize) + 1);
	
	// Update bitMap by zeroing out what was once used space, now it can be written over by new files.
	for(int i = (entryList + fileIndex)->location; i < ((entryList + fileIndex)->location + (entryList + fileIndex)->count); i++)
	{
		*(bitMap + i) = '0';
	}
	
	//To write bitmap and entries, trackPosition starts at 1 since we don't need to write vcb again
	int trackPosition = 1;
	int bitMapBlksWritten = LBAwrite(bitMap, (((sizeof(char) * lbaCount) / blockSize) + 1), 1);
	trackPosition += (((sizeof(char) * lbaCount) / blockSize) + 1);
	
	// Update entryList by nulling entry
	(entryList + fileIndex)->parent = -2;
	strcpy((entryList + fileIndex)->name, "-1");
	(entryList + fileIndex)->count = -1;
	(entryList + fileIndex)->location = -1;
	(entryList + fileIndex)->index = -1;
	//(entryList + fileIndex)->id =
	(entryList + fileIndex)->bitMap = -1;
			
	int entryListBlksWritten = LBAwrite(entryList, (((sizeof(entry) * lbaCount) / blockSize) + 1) , trackPosition);
	trackPosition += (((sizeof(entry) * lbaCount) / blockSize) + 1);
	
	return 0;
}
