/**
 * Lê mensagens de um FIFO e exibe na tela
 */
#include <stdio.h>      // standard io
#include <stdlib.h>     // standard lib
#include <errno.h>      // number of last error
#include <sys/stat.h>   // data returned stat() function (mkfifo)
#include <unistd.h>     // unix standard
#include <fcntl.h>      // file control options
#include <string.h>     // string operations

#define SERVER_FIFO "/tmp/serverfifo"

int main (int argc, char **argv)
{
    int fd_server, num_bytes_read;
    char buf [512];

    // cria um FIFO se inexistente
    if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }

    // abre um FIFO
    if ((fd_server = open (SERVER_FIFO, O_RDONLY)) == -1)
        perror ("open");

    // lê e trata mensagens do FIFO 
     while (1) {
         memset (buf, '\0', sizeof (buf));
         num_bytes_read = read (fd_server, buf, sizeof (buf));
         switch (num_bytes_read) {
             case -1: 
                perror ("-- read error"); break;
             case  0:  
                printf("-- None data...close and reopen fifo --\n");
                close(fd_server);
                fd_server = open (SERVER_FIFO, O_RDONLY); 
                break;
             default: 
                printf("Received %d bytes: %s\n", num_bytes_read, buf);
         }
     }

     if (close (fd_server) == -1)
        perror ("close");
}

