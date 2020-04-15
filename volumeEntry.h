typedef struct VolumeEntry { 
    int numLba;
    int freeInfoLocation; //for '3' copies of free info locations in volume
    int rootdirLocation; //location of root directory
    char volumeName[128];
    unsigned int volumeID;
    unsigned int lbaSize;// = 512;
    unsigned int clusterSize;// = 8;
} volumeEntry;

typedef struct freeChunk {
    char signature1[8];
    
    unsigned int inUse;
    unsigned int location;
    //int next; //index of next chunk in LBA
    //int prev; //index of prev chunk in LBA
    long count;

    char signature2[8];
} freeChunk;

//opposite of free space
typedef struct extent {
    char signature1[8];

    unsigned int inUse;
    unsigned int location;
    int fileID; //can be directory as well
    //int next; //next chunk of file
    //int prev; //prev chunk of file
    long count;

    char signature2[8];
} usedChunk;