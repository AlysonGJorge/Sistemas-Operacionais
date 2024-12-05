#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matriz.h"

/*
    Cabeçalho do código ex1.c
    Criado por: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 03/11/2024
    Finalidade: algoritmo que lê uma matriz aleatoria e realiza a soma de todos os elementos utilizando threads com mutex;
*/

int **matriz;
int linhaAtual = 0;
int somaTotal = 0;
int linhasTotais, colunasTotais, threadsTotais;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int threadId;
} threadData;


    /**
        * thread_somadora 
        * realiza a soma de uma linha da matriz utilizando mutex
        * parâmetros: arg - pra saber qual thread está realizando a soma
    */
void* thread_somadora(void* arg) {
    threadData Data = *((threadData *)arg);
    int local_sum = 0;

    while (1) {
        int linhaSendoLida;

        pthread_mutex_lock(&mutex);
        if (linhaAtual >= linhasTotais) {
            pthread_mutex_unlock(&mutex);
            break; // todas as linhas já foram processadas
        }
        linhaSendoLida = linhaAtual;
        linhaAtual++;
        printf("Eu, a thread %d, estou realizando a soma da linha %d\n", Data.threadId, linhaSendoLida);

        for (int j = 0; j < colunasTotais; j++) {
            somaTotal += matriz[linhaSendoLida][j];
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    printf("Olá, eu irei realizar a soma de todos os elementos de uma matriz utilizando threads com mutex\n");
    printf("me diga o número de colunas da matriz!\n");
    scanf("%d", &colunasTotais);
    printf("me diga o número de linhas da matriz!\n");
    scanf("%d", &linhasTotais);
    printf("me diga o número de threads que deseja criar!\n");
    scanf("%d", &threadsTotais);
    pthread_t threads[threadsTotais];
    int thread_args[threadsTotais];

    // inicializa matriz
    matriz = create_matrix(linhasTotais, colunasTotais);
    // insere valores na matriz
    generate_elements(matriz, linhasTotais, colunasTotais, 100);
    print_matrix(matriz, linhasTotais, colunasTotais);

    // cria as threads
    for (int i = 0; i < threadsTotais; i++) {
        threadData Data;
        Data.threadId = i;
        thread_args[i] = i; // Define o ID da thread
        int resultado = pthread_create(&threads[i], NULL, thread_somadora, (void *)&Data);
        if (resultado != 0) {
            fprintf(stderr, "Erro ao criar a thread %d\n", i);
            return EXIT_FAILURE;
        }
    }

    // espera todas as threads terminarem
    for (int i = 0; i < threadsTotais; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(matriz);

    printf("Soma total: %d\n", somaTotal);

    return 0;
}
