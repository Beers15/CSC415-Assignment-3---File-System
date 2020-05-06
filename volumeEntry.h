typedef struct VolumeEntry { 
    char startOfVolume;
    int numLba;
    int rootdirLocation; //location of root directory
    char volumeName[128];
    unsigned int volumeID;
    unsigned int lbaSize;
} volumeEntry;