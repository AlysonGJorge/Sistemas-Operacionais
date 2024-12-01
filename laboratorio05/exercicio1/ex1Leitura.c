#include <stdio.h>      // standard io
#include <stdlib.h>     // standard lib
#include <errno.h>      // number of last error
#include <sys/stat.h>   // data returned stat() function (mkfifo)
#include <unistd.h>     // unix standard
#include <fcntl.h>      // file control options
#include <string.h>     // string operations

#define SERVER_FIFO "/tmp/serverfifo"

void fodase(char* buffer){
        printf("Envie uma mensagem! \n");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove o caractere de nova linha
    printf("Sua mensagem é:\n");
    printf("%s\n", buffer);
}

void contaCaracteres(char* buffer){
    int posicaoString = 0;
    int nrConsoantes = 0;
    int nrVogais = 0;
    int nrEspacos = 0;
    
    while (buffer[posicaoString] != '\0')
    {
        if (buffer[posicaoString] == 'a' || buffer[posicaoString] == 'e' || buffer[posicaoString] == 'i' || buffer[posicaoString] == 'o' || buffer[posicaoString] == 'u')
        {
            nrVogais++;
        }
        else if (buffer[posicaoString] == ' ')
        {
            nrEspacos++;
        }
        else if (buffer[posicaoString] >= 'a' && buffer[posicaoString] <= 'z' || buffer[posicaoString] >= 'A' && buffer[posicaoString] <= 'Z')
        {
            nrConsoantes++;
        }
        posicaoString++;
    }
    printf("Número de caracteres: %d\n", posicaoString);
    printf("Número de vogais: %d\n", nrVogais);
    printf("Número de consoantes: %d\n", nrConsoantes);
    printf("Número de espaços: %d\n", nrEspacos);
    return;
}

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
                contaCaracteres(buf);
        }
    }

    if (close (fd_server) == -1)
        perror ("close");
}