/**
 * Escreve N mensagens para o FIFO
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/serverfifo"
#define N 5

char buf [512];

int main (int argc, char **argv)
{
    int fd_server;  // descritor para o FIFO

    if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
        perror ("open error: server fifo");
        return 1;
    }

    int value = 0;
    while (value < N) {
        // cria mensagens incrementalmente
        sprintf (buf, "Message %ld - Number %d", (long) getpid (), value++);
        printf("Sending: %s\n", buf);

        // envia mensagem para o fifo
        write (fd_server, buf, strlen (buf));
    
        sleep(1);  // somente para visualizacao
    }
        
    if (close (fd_server) == -1) {
        perror ("close error:server fifo");
        return 2;
    }
          
    return 0;
}

