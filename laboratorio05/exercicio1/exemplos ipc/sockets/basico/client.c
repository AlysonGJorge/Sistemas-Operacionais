/**
 * @file client.c
 * @author Rodrigo Campiolo
 * @brief Exemplo básico de comunicação com sockets inet/stream
 * 		  Conecta ao servidor, envia msg e aguardo ok do servidor.
 * @version 0.1
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

#define PORTA_SERVER 30000

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Sintaxe:\nclient IPSERVIDOR");
        return 1;
    }       

	int client_socket, numbytes;
	int bufsize = 1024;
	char *buffer = malloc(bufsize);
	struct sockaddr_in server_address;

	printf("Criando socket...\n");
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
		printf("Socket criado.\n");
	} else {
		perror("Erro criando o socket.");
		exit(1);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORTA_SERVER);
	inet_pton(AF_INET, argv[1], &server_address.sin_addr);

	printf("Tentando se conectar ao servidor: %s\n", argv[1]);

	// conecta ao servidor
	if (connect (client_socket, (struct sockaddr *)&server_address,
	     		sizeof(server_address)) == 0) {
		// exibe endereço do servidor
		printf("Conexão estabelecida com o servidor %s...\n",
		       inet_ntoa(server_address.sin_addr));
	} else {
		perror("Erro conectando ao servidor.");
		exit(1);
	}

	// le e envia mensagem
	printf("Mensagem: %s\n", buffer);
	fgets(buffer, bufsize - 1, stdin);
	send(client_socket, buffer, strlen(buffer), 0);

	// aguarda mensagem ok do servidor
	if ((numbytes = recv(client_socket, buffer, bufsize, 0)) == -1) {
		perror("Erro no receive");
		exit(1);
	}

	buffer[numbytes] = '\0';
	printf("Mensagem recebida: %s\n", buffer);

	// fecha o socket
	close(client_socket);

	return 0;
}
