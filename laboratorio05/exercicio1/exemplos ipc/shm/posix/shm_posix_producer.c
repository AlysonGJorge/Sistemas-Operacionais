/**
 * Simple program demonstrating shared memory in POSIX systems.
 * This is the producer process that writes to the shared memory region.
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
	const int SIZE = 4096;
	const char *name = "OS";
	const char *message1= "Operating Systems ";
	const char *message2= "Is Fun!";

	int shm_fd;   // descritor segmento de memoria compartilhada
	void *ptr;    // ponteiro segmento de memoria compartilhada

	/* cria segmento de memoria compartilhada */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configura o tamanho do segmento */
	ftruncate(shm_fd, SIZE);

	/* mapeia o segmento para o espaco de enderecamento do processo */
	ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

	/* escreve para a segmento de memoria compartilhada	
	 * obs: incrementa-se ponteiro a cada escrita */
	sprintf(ptr,"%s",message1);
	ptr += strlen(message1);
	sprintf(ptr,"%s",message2);
	ptr += strlen(message2);

	return 0;
}
