 
/*
    Programa que solicita uma entrada composta por números, envia para um servidor via FIFO e
    recebe a soma.
    source: https://www.softprayog.in/programming/interprocess-communication-using-fifos-in-linux

    Modificado por: Rodrigo
    Data: 02/10/2023

 */
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"

char my_fifo_name [128];
char buf1 [512], buf2 [1024];

int main (int argc, char **argv)
{
    int fd, fd_server, bytes_read;

    // cria o fifo para a comunicação concatenando como PID do processo.
    sprintf (my_fifo_name, "/tmp/add_client_fifo_%ld", (long) getpid ());

    if (mkfifo (my_fifo_name, 0664) == -1)
        perror ("mkfifo");

    printf("-- SOMA SEQUÊNCIA DE NÚMEROS --\n\n");
    while (1) {
        // Solicita ao usuário uma sequência de números
        printf ("Digite uma sequência: \n");
        if (fgets(buf1, sizeof (buf1), stdin) == NULL)
            break;

        strcpy (buf2, my_fifo_name);
        strcat (buf2, " ");
        strcat (buf2, buf1);

        // abre o FIFO 
        if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
            perror ("open: server fifo");
            break;
        }
        
        // envia a sequência ao servidor
        if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
            perror ("write");
             break;
        }

        // fecha o FIFO
        if (close (fd_server) == -1) {
            perror ("close");
            break;
        }

        // abre o FIFO para ler a resposta do servidor
        if ((fd = open (my_fifo_name, O_RDONLY)) == -1)
           perror ("open");
        
        memset (buf2, '\0', sizeof (buf2));
        
        // lê a resposta do servidor
        if ((bytes_read = read (fd, buf2, sizeof (buf2))) == -1)
            perror ("read");

        if (bytes_read > 0) {
            printf ("Answer: %s\n", buf2);
        }

        // fecha o FIFO
        if (close (fd) == -1) {
            perror ("close");
            break;
        }
    }
    
    return 0;
}
