#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_NAME "disk"
#define DISK_SIZE "1024000"             //1M

#define BEGIN_METADATA          0
#define END_METADATA            49      // 50B
#define BEGIN_FREE_SPACE        50
#define END_FREE_SPACE          99      // 50B
#define BEGIN_INODE             100
#define END_INODE               2099    // 2KB
#define BEGIN_BLOCK             2100
#define END_BLOCK               2599    // 500B
#define BEGIN_INODEMAP          2600
#define END_INODEMAP            2649    // 50B

#define INODE_MAX               50      // 50
#define INODE_SIZE              2000    // 2KB
#define INODE_BYTE              40      // 40B
#define INODE_LENGTH            2       // 1 - 50 

#define INODE_PARENT            0
#define INODE_ID                2
#define INODE_ID_SIZE           2
#define INODE_NAME              4
#define INODE_NAME_SIZE         20
#define INODE_DATE              24
#define INODE_BLOCK1            32
#define INODE_BLOCK2            36
#define INODE_BLOCK_SIZE        4

#define INODE_MAP_MAX           50          

char *disk;
char *size;                             // 2600B
char *metadataInit;

int nextPosition(char *disk);
int positionMap(char *disk , int posBegin, int posEnd);
void bootLoader(char *disk);
void setPosition(char *disk);
void createDir(char *disk, char *path);
void createFile(char *disk, char *file_name);
void writeDir(int inode_address, char *inode_parent,  char *inode_id);
void writeInodeParent(char *inode_parent, char *inode_son);

char *checkDir(char *disk, char *path);
int checkInodeMap(char *disk);
char *getInodeId(int inode_address);