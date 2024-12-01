/*
 * Fonte: http://www.cs.cf.ac.uk/Dave/C/node27.html
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* Tamanho do segmento compartilhado. */
#define SHMSZ     128

int main() {
	int shmid;        // id segmento memoria compartilhada
	key_t key;        // chave de acesso memoria compartilhada
	char *shm;        // ponteiro para memória compartilhada 

	/* ID segmento de memória compartilhada */
	key = 5678;

	/* Cria o segmento compartilhado. */
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
		perror("Erro ao criar o segmento de shm (shmget).");
		exit(1);
	}
	printf ("ID regiao: %d. \nUse ipcs -m \n", shmid);

	/* Acoplamento do segmento criado ao espaco do processo */
	if ((shm = shmat(shmid, NULL, 0)) == (char*)-1) {
		perror("Erro ao acoplar o segmento ao processo (shmat).");
		exit(1);
	}

	/* Produzindo dados */
	strncpy(shm, "Olá Mundo!", 10);
	shm[10] = '\0';

	/* Aguardando a leitura do outro processo (espera ocupada). 
	  Consumidor rá mudar o primeiro '*' indicando leitura */
	while (*shm != '*')
		sleep(1);

	/* Desacoplamento da região de memória compartilhada. */
	if (shmdt(shm) == -1) {
		perror("Erro ao desacoplar o segmento (shmdt).");
		exit(1);
	}

    /* Destruição do segmento */
    if (shmctl(shmid, IPC_RMID, 0) == -1){
        perror("Erro ao destruir o segmento (shmctl).") ;
        exit(1) ;
    }
    
	return EXIT_SUCCESS;
}