#include <time.h>
#include <sys/types.h>// for id_t
#include <stdint.h>

typedef struct entry {
	char name[128];
	unsigned long size; //number of logical blocks
	int location; //block number in LBA
	int index;
	id_t id;
	time_t createTime;
	time_t lastModified;
	uint16_t bitMap; //various binary values would represent if its a file, directory, or alias
	int parent; //parent block number
	//uint32_t permissions;
	int count;
} entry;