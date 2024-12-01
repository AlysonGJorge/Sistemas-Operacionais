/**
 * Envia mensagens para um processo servidor com sockets UNIX
 * SOURCE: https://troydhanson.github.io/network/Unix_domain_sockets.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/* especificar o path para mapear o socket */
//char *socket_path = "./mysocket";
char *socket_path = "\0myabstractsocket";

int main(int argc, char *argv[]) {
    int client_socket,          // descritor para o socket (file descriptor)
        sent_bytes;             // número de bytes enviados 
    struct sockaddr_un addr;    // estrutura de endereço socket unix
    char buf[100];              // buffer para troca de mensagens
    
    /* socket_path por parâmetro */
    if (argc > 1) socket_path = argv[1];

    /* cria socket UNIX do tipo SOCK_STREAM */
    if ( (client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    /* preenche estrutura de endereço */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (*socket_path == '\0') {   // socket não mapeado no sistema de arquivos (abstract)
        *addr.sun_path = '\0';
        strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
    } else {
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    }

    /* conecta com o processo servidor */
    if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }

    /* lê dados do terminal e envia via sockets */
    while ( (sent_bytes = read(STDIN_FILENO, buf, sizeof(buf)) ) > 0) {
        if (write(client_socket, buf, sent_bytes) != sent_bytes) {
            if (sent_bytes > 0) fprintf(stderr,"partial write");
            else {
                perror("write error");
                exit(-1);
            }
        }
    }

    return 0;
}
