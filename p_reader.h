#ifndef READER_H
#define READER_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <pthread.h>
#include <string.h>
#include "filesystem.h"

void *pipeReaderFile();
void *pipeReaderDir();

pthread_t tId[2];

#endif