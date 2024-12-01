 
/**
 *    Servidor que adiciona os números recebidos e devolve ao cliente.
 *    Exemplo:
 *     Entrada: 1,2,3,4
 *     Saída: Soma = 10
 *
 *    Source: https://www.softprayog.in/programming/interprocess-communication-using-fifos-in-linux 
 *    Modificado por: Rodrigo Campiolo
 *    Data: 02/10/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"
#define MAX_NUMBERS 500

/**
 * Processa um vetor de char com elementos numéricos e devolve a soma
 * elements: elementos numéricos em um vetor de strings
 * total_numbers: quantidade de elementos
 * error: variável para devolver o código de erro (se ocorrer)
 */
double sum_elements (char **elements, int total_numbers, unsigned int* error) {
    double f, sum = 0;
    char *ptr;

    for (int i = 0; i < total_numbers; i++) {
        f = strtod (elements[i], &ptr);

        if (*ptr) {
            *error = 1;
            return -1;
        }
        sum += f;
    }

    return sum;
}

/**
 * Executa um servidor para somar vetores de números recebidos via FIFO 
 * e devolve a soma para o processo solicitante.
 */
int main (int argc, char **argv)
{
    int fd, fd_client,              // descritores do FIFO (servidor e cliente)
        bytes_read, i;              
    char buf [4096];
    char *return_fifo;              // nome do FIFO do cliente
    char *numbers [MAX_NUMBERS];

    printf ("-- SERVIDOR iniciado.\n\n");

    // cria um FIFO
    if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }

    // abre o FIFO para leitura
    if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1)
        perror ("open");

     while (1) {
         // aguarda mensagens com um buf de char
         memset (buf, '\0', sizeof (buf));
         if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
             perror ("read");
         if (bytes_read == 0)
             continue;

         // caso tenha uma entrada, processe a soma    
         if (bytes_read > 0) {
             return_fifo = strtok (buf, ", \n");
             printf("Solicitação de %s\n", return_fifo);

             // parser: vírgula, espaço e fim de linha   
             i = 0;
             numbers [i] = strtok (NULL, ", \n");
             while (numbers [i] != NULL && i < MAX_NUMBERS)
                 numbers [++i] = strtok (NULL, ", \n");

             // invoca a somatoria dos valores    
             unsigned int error = 0;
             double sum = sum_elements(numbers, i, &error);

             // abre o fifo para a escrita
             if ((fd_client = open (return_fifo, O_WRONLY)) == -1) {
                 perror ("open: client fifo");
                 continue;
             }

             if (error) sprintf (buf, "Erro na entrada.\n");
             else sprintf (buf, "Soma = %.8g\n", sum);

            //  envia o resultado (soma) para o cliente
             if (write (fd_client, buf, strlen (buf)) != strlen (buf))
                 perror ("write");

             if (close (fd_client) == -1)
                 perror ("close");
         }

     }
}
