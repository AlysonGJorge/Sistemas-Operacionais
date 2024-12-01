/**
 * @file server.c
 * @author Rodrigo Campiolo
 * @brief Exemplo básico de comunicação com sockets inet/stream
 * 		  Aguarda conexoes, recebe msg e devolve ao cliente (echo).
 * @version 0.1
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/types.h>	// definicoes de tipos (p.ex. pid_t)
#include <sys/socket.h> // protocolos para internet (accept, bind, connect, listen, ...)
#include <netinet/in.h> // protocolos para internet (sockaddr_in)
#include <arpa/inet.h>	// htonl/htons/ntohl/ntohs
#include <unistd.h>		// unix standard
#include <stdlib.h>		// lib standard
#include <stdio.h>		// io standard
#include <errno.h>      // erros
#include <string.h>		// manipulacao de strings

#define PORTA 30000
#define CONEXOES 10 // backlog = tamanho maximo fila de conexoes pendentes
					// padrao do sistema: /proc/sys/net/core/somaxconn

int main() {

	int server_socket, 	// socket para aguardar conexoes
		client_socket,  // socket para comunicar com cliente
		addrlen;		// auxiliar para endereco

	int bufsize = 1024; 
	char *buffer = malloc(bufsize);      // buffer

	struct sockaddr_in server_address; // endereco do servidor
	struct sockaddr_in client_address; // endereco do cliente

	printf("Iniciando servidor...\n");

	/* criar um socket da familia Internet e comunicação por fluxo */
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
		printf("Socket criado.\n");
	} else {
		perror("Erro na criação do socket.");
		exit(1);
	}

	// configurar endereço do servidor.
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;	// Coloca IP automaticamente.
	server_address.sin_port = htons(PORTA);
	bzero(&(server_address.sin_zero), 8);

	/* mapear o socket para uma porta */
	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr)) == 0) {
		printf("Binding Socket.\n");
	} else {
		perror("Erro ao executar o Binding.");
		exit(1);
	}

	/* configurar o socket como passivo (aguardar conexoes)*/
	printf("Listening...\n");
	if (listen(server_socket, CONEXOES) < 0) {
		perror("Erro no listen.");
		exit(1);
	}

	while(1) {
		/* aguarda conexoes dos clientes e  devolve socket de comunicacao */
		addrlen = sizeof(struct sockaddr_in);
		printf("Aceitando conexões de clientes...\n");
		if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address,
				(socklen_t *)&addrlen)) < 0) {
			printf("Erro no accept.\n");
		}

		/* exibe o endereco do cliente */
		if (client_socket > 0) {
			printf("Cliente %s está conectado...\n", inet_ntoa(client_address.sin_addr));
		}
	
		/* aguarda mensagem do cliente */
		int bytes_lidos = recv(client_socket, buffer, bufsize, 0);
		buffer[bytes_lidos] = '\0';
		printf("Mensagem recebida: %s\n", buffer);

		/* envia mensagem "OK" cliente */
		strcpy(buffer, "OK");
		send(client_socket, buffer, strlen(buffer), 0);

		/* fecha o socket do cliente*/
		close(client_socket);
	}

	/* fecha o socket do servidor */
	close(server_socket);

	return 0;
}
