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
    char signature1[8];

    int next; //index of next chunk in LBA
    int prev; //index of prev chunk in LBA
    long count;

    char sig2[8];
};

//opposite of free space
struct extents {
    char signature1[8];

    int fileID; //can be directory as well
    int next; //next chunk of file
    int prev; //prev chunk of file
    long count;

    char signature2[8];
};