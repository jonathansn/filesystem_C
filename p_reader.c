#include "p_reader.h"


void *pipeReaderDir(){
    
    int fd;
    char *myFifo = "/tmp/pipeDir";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
        fd = open(myFifo, O_RDONLY);
        read(fd, buffer, 1024);
        if(strlen(buffer) != 0){
            printf("[+] Path: %s\n", buffer);
            if(createDir("disk", buffer) == 1){
                printf("Directory created successfully!\n");
            } else {
                printf("Error while trying to create directory!\n");
            }
            memset(buffer,0,strlen(buffer));
            close(fd);
        }
}

void *pipeReaderFile(){
    
    int fd;
    char *myFifo = "/tmp/pipeFile";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
        fd = open(myFifo, O_RDONLY);
        read(fd, buffer, 1024);
        if(strlen(buffer) != 0){
            printf("[+] Path/File: %s\n", buffer);
            if(createFile("disk", buffer, "testandodenovo") == 1){
                printf("File created successfully!\n");
            } else {
                printf("Error while trying to create file!\n");
            }
            memset(buffer,0,strlen(buffer));
            close(fd);
        }
}


