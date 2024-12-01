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

    while (1) {
        // cria mensagens incrementalmente
        
        printf("Envie uma mensagem! \n");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0'; // Remove o caractere de nova linha
    printf("Sua mensagem é:\n");
    printf("%s\n", buf);

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

