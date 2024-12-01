/*
 * Fonte: http://www.cs.cf.ac.uk/Dave/C/node27.html
 */
/*
 * shm-client - client program to demonstrate shared memory.
 * 
 * int shmget(key_t key, size_t size, int shmflg): devolve identificador do segmento de memória compartilhada
 * void *shmat(int shmid, const void *shmaddr, int shmflg): acopla o segmento de memória compartilhada  ao espaço do processo.
 * int shmdt(const void *shmaddr);: desacopla o segmento de memória compartilhada.
 *  shmctl(shmid, IPC_RMID, NULL): remove um segmento de memória compartilhada.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSZ     128

int main() {
	int shmid;
	key_t key;
	char *shm, *s;

	/* chave do segmento criado pelo produtor. */
	key = 5678;

	/* Localizando o segmento */
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
		perror("Erro ao acessar o segmento de shm (shmget).");
		exit(1);
	}

	/* Acoplamento do segmento ao processo. */
	if ((shm = shmat(shmid, NULL, 0)) == (char*)-1) {
		perror("Erro ao acoplar o segmento ao processo (shmat).");
		exit(1);
	}

	/* leitura do segmento de memoria compartilhado */
	for (s = shm; *s != '\0'; s++)
		putchar(*s);
	putchar('\n');

	/* Modificando o primeiro caracter do segmento para '*',
	   indicando que os dados já foram lidos. */
	*shm = '*';

	/* Desacopla a região de memória compartilhada. */
	if (shmdt(shm) == -1) {
		perror("Erro ao desacoplar o segmento(shmdt).");
		exit(1);
	}

	return EXIT_SUCCESS;
}
