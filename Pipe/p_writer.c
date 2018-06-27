#include "p_writer.h"

void pipeWriter(){
    int fd;
    char *myFifo = "/tmp/myfifo";

    /* remove the Fifo if it already exist */
    unlink(myFifo);

    /* create the FIFO (named pipe) */
    mkfifo(myFifo, 0666);

    /* write to the FIFO */
    fd = open(myFifo, O_WRONLY);
    write(fd, "Hi", sizeof("Hi"));
    close(fd);

    /* remove the FIFO */
    unlink(myFifo);

    return 0;

}