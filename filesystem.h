#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_NAME "disk"
#define DISK_SIZE "1024000"       //1M

char *disk;
char *size;

int nextPosition(char *disk);
void setPosition(char *disk);
void createFile(char *disk, char *file);