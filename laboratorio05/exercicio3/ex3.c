/*
* Implemente um programa que realize a soma de vetores utilizando processos para fazer o cálculo, mas
com os vetores sendo compartilhados pelos processos. Como os espaços de memória entre os processos
são isolados, um mecanismo fornecido pelo SO deve ser usado. No caso, use memória compartilhada
para que todos os filhos operem sobre os dados, e pipes para a realização do despacho de trabalho
(intervalo de índices no vetor). O número de elementos do vetor e o número de processos filhos deve ser
fornecido pelo usuário. Por exemplo, numElementos = 1000 e numProcessos = 5, cada filho processará
200 índices; para numElementos = 1000 e numProcessos = 4, cada filho processará 250 índices.
* @author: Alysson Gonçalves Jorge, Hudson Taylor Perrut Cassim, Natanael Aparecido Tagliaferro Galafassi.
* @date 02/12/2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

/* Programa principal */
int main(void) {
	pid_t pid;
    int buffer[2];
    int tamVetor, numProcessos, parteInteira, parteCompleta, status; 

    FILE *file = fopen("vetores.txt", "r+");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return EXIT_FAILURE;
    }

    // Lendo os tamanhos dos vetores
    fscanf(file, "%d", &tamVetor);
    int vetor1[tamVetor];
    int vetor2[tamVetor];

    for (int i = 0; i < tamVetor; i++) {
        fscanf(file, "%d", &vetor1[i]);
    }
    for (int i = 0; i < tamVetor; i++) {
        fscanf(file, "%d", &vetor2[i]);
    }
    
    printf("\nNúmero de processos:  ");
    scanf("%d", &numProcessos);

    parteCompleta = 0;
    parteInteira = tamVetor/numProcessos;
    if (tamVetor % numProcessos)
        parteCompleta = parteInteira + tamVetor % numProcessos;

    int mypipes[numProcessos][2];

    for (int i = 0; i < numProcessos; i++)
    {
        if (i == 0 || pid != (pid_t) 0)
        {
            /* Criar o pipe. */
            if (pipe(mypipes[i])) {
                fprintf(stderr, "Falha ao criar o Pipe.\n");
                return EXIT_FAILURE;
            }
            /* Criar o processo filho. */
            pid = fork();
            if (pid < (pid_t) 0) {
                /* pid negativo, falha no fork. */
                fprintf(stderr, "Falha ao executar o Fork.\n");
                return EXIT_FAILURE;
            } else if (pid == (pid_t) 0) {
                /* No processo filho. */
                close(mypipes[i][1]);
                read(mypipes[i][0], buffer, sizeof(buffer));

                const char *name = "EX3";
                const int SIZE = (sizeof(int) * tamVetor *3);

                int shm_fd;   // descritor segmento de memoria compartilhada
                int *ptr;    // ponteiro segmento de memoria compartilhada

                /* abre segmento de memoria compartilhada*/ 
                shm_fd = shm_open(name, O_RDWR, 0666);
                if (shm_fd == -1) {
                    printf("shared memory failed\n");
                    exit(-1);
                }

                /* mapeia segmento no espaco de enderecamento do processo */ 
                ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                if (ptr == MAP_FAILED) {
                    printf("Map failed\n");
                    exit(-1);
                }

                int pos = buffer[0] / parteInteira;
                printf("Processo %d:\n", pos);
                for (int j = buffer[0]; j <= buffer[1]; j++){
                    ptr[tamVetor*2 + j] = ptr[j] + ptr[tamVetor + j];
                    printf("%d + %d = %d ", ptr[j], ptr[tamVetor + j], ptr[tamVetor*2 + j] );
                    printf("\n");
                }

                const char *name2 = "Confirma";
                int size2 = numProcessos;
                int shm_fd2;   // descritor segmento de memoria compartilhada
                char *ptr2;    // ponteiro segmento de memoria compartilhada

                /* abre segmento de memoria compartilhada*/ 
                shm_fd2 = shm_open(name2, O_RDWR, 0666);
                if (shm_fd2 == -1) {
                    printf("shared memory failed\n");
                    exit(-1);
                }

                /* mapeia o segmento para o espaco de enderecamento do processo */
                ptr2 = mmap(NULL, size2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
                if (ptr2 == MAP_FAILED) {
                    printf("Map failed\n");
                    return -1;
                }

               
                ptr2[pos] = 'Y';
                exit( 0 ); 
            }
        } 
    }

	const char *name = "EX3";
    int size = (sizeof(int) * tamVetor *3);
	int shm_fd;   // descritor segmento de memoria compartilhada
	int *ptr;    // ponteiro segmento de memoria compartilhada

	/* cria segmento de memoria compartilhada */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configura o tamanho do segmento */
	ftruncate(shm_fd, size);

	/* mapeia o segmento para o espaco de enderecamento do processo */
	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

    /* escreve para a segmento de memoria compartilhada	
	 * obs: incrementa-se ponteiro a cada escrita */
    memcpy(ptr, vetor1, sizeof(vetor1));
    memcpy(&ptr[tamVetor], vetor2, sizeof(vetor2));

    const char *name2 = "Confirma";
    int size2 = numProcessos;
	int shm_fd2;   // descritor segmento de memoria compartilhada
	char *ptr2;    // ponteiro segmento de memoria compartilhada

	/* cria segmento de memoria compartilhada */
	shm_fd2 = shm_open(name2, O_CREAT | O_RDWR, 0666);

	/* configura o tamanho do segmento */
	ftruncate(shm_fd2, size2);

	/* mapeia o segmento para o espaco de enderecamento do processo */
   
	ptr2 = mmap(NULL, size2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
	if (ptr2 == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

    for (int i = 0; i < numProcessos; i++)
    {
        ptr2[i] = 'N';    
    }

    for (int i = 0; i < numProcessos; i++)
    {
        close(mypipes[i][0]);
        if (i ==0 && parteCompleta)
        {
           buffer[0] = i;
           buffer[1] = parteCompleta - 1;
        }else{
            buffer[0] = i * parteInteira;
            buffer[1] = (parteInteira * (i+1)) - 1;
        }
        write(mypipes[i][1], buffer, sizeof(buffer));
    }

    for (int i = 0; i < numProcessos; i++)
    {
        while (ptr2[i] != 'Y');
    }
    fseek(file, 0, SEEK_END);
    fprintf(file, "\nVetor resultante:\n");
    for (int i = tamVetor*2; i < tamVetor*3; i++)
    {
        fprintf(file, "%d ", ptr[i]);
    }
    fclose(file);
    return 0;

}
