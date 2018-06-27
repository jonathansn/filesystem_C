#include "p_reader.h"

void pipeReader(){
    
    int fd;
    char *myFifo = "/tmp/myfifo";
    char buffer[1024];

    /* opem, read and display the message from the FIFO */
    fd = open(myFifo, O_RDONLY);
    read(fd, buffer, 1024);
    printf("[+] Received: %s\n", buffer);
    close(fd);

    return 0;

}