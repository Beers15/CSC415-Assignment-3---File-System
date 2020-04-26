#include <time.h>
#include <sys/types.h>// for id_t
#include <stdint.h>

typedef struct entry {
	char name[128];
	int location; //block number in LBA
	int index;
	id_t id;
	uint32_t bitMap; //to mark entry as used or unused
	time_t createTime;
	time_t lastModified;
	int parent; //parent entryList index
	//uint32_t permissions;
	int count; //number of logical blocks
} entry;