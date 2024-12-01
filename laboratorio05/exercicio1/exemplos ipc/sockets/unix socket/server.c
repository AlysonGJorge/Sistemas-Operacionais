/**
 * Aguarda mensagens de um processo cliente com sockets UNIX
 * SOURCE: https://troydhanson.github.io/network/Unix_domain_sockets.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

//char *socket_path = "./mysocket";
char *socket_path = "\0myabstractsocket";

int main(int argc, char *argv[]) {
    int server_socket,       // descritor do socket
        client_socket,       // socket da conexao do cliente
        received_bytes;      // bytes recebidos
    struct sockaddr_un addr; // endereço socket 
    char buf[100];           // buffer de comunicação

    if (argc > 1) socket_path=argv[1];

    /* cria um socket AF_UNIX do tipo SOCK_STREAM */
    if ( (server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    /* configura endereço do socket */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    
    if (*socket_path == '\0') {
        *addr.sun_path = '\0';
        strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
    } else {
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
        unlink(socket_path);  // desvincular path se existe
    }

    /* mapeia o socket para o socket_path */
    if (bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(-1);
    }

    /* configura para aguardar conexões */
    if (listen(server_socket, 5) == -1) {
        perror("listen error");
        exit(-1);
    }

    while (1) {
      /* aguarda conexões dos clientes */
      if ( (client_socket = accept(server_socket, NULL, NULL)) == -1) {
          perror("accept error");
          continue;
      }

      /* lê dados envidos pelos clientes */
      while ( (received_bytes = read(client_socket, buf, sizeof(buf))) > 0 ) {
          printf("read %u bytes: %.*s\n", received_bytes, received_bytes, buf);
      }
      
      /* trata erros */
      if (received_bytes == -1) {
          perror("read");
          exit(-1);
      } else if (received_bytes == 0) {
          printf("EOF\n");
          close(client_socket);
      }
    }

    return 0;
}
