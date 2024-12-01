/**
 * Simple program demonstrating shared memory in POSIX systems.
 * This is the consumer process
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
	const char *name = "OS";
	const int SIZE = 4096;

	int shm_fd;   // descritor segmento de memoria compartilhada
	void *ptr;    // ponteiro segmento de memoria compartilhada

	/* abre segmento de memoria compartilhada */
	shm_fd = shm_open(name, O_RDONLY, 0666);
	if (shm_fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* mapeia segmento no espaco de enderecamento do processo */
	ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	/* le segmento como uma string */
	printf("%s", (char *)ptr);

	/* remove segmento de memoria compartilhada */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}

	return 0;
}
