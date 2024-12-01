/** Demonstracao de como usar unnamed sockets para comunicação entre
    processos pai e filho.
    int socketpair(int domain, int type, int protocol, int sv[2]); 
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define HELLOREQUEST  "Hello, how are you?"
#define HELLORESPONSE "I am fine, thanks."

int main()
{
   int sockets[2], pid;
   char buf[1024];
   
   /* cria um par de sockets de domínio UNIX e tipo STREAM */
   if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
      perror("opening stream socket pair");
      exit(1);
   }
   
   pid = fork();
     
   if (pid) {     /* Processo PAI */
      /* PAI usa o sockets[1] para enviar/receber */ 
      close(sockets[0]);      
      
      if (read(sockets[1], buf, 1024) < 0)
         perror("error reading message");
      
      printf("  PAI recv: %s\n", buf);
      
      if (write(sockets[1], HELLORESPONSE, sizeof(HELLORESPONSE)) < 0)
         perror("error writing message");
      
      close(sockets[1]);

   } else {       /* Processo FILHO. */
      /* FILHO usa o sockets[0] para enviar/receber */ 
      close(sockets[1]);
      
      if (write(sockets[0], HELLOREQUEST, sizeof(HELLORESPONSE)) < 0)
         perror("error writing message");
      
      if (read(sockets[0], buf, 1024) < 0)
         perror("error reading message");
      
      printf("FILHO recv: %s\n", buf);
      
      close(sockets[0]);
   }
   
   return 0;
} 
