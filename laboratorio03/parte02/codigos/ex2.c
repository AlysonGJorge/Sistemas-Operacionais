#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <math.h>
#include "matriz.h"
#include <time.h>

typedef struct {
    int **matrix;
    int threadId;
    int* TotalLinhas;
    int* TotalColunas;

    int comecoLinhaAritmetica;
    int finalLinhaAritmetica;

    int comecoColunaGeometrica;
    int finalColunaGeometrica;

    int restoAritmetica;
    int restoGeometrica;

    double* mediasAritmetica;
    double* mediasGeometrica;

} threadData;

double calculaMediaAritmetica(int linhaDaMatriz, int colunasTotais, int** matrix){
    double soma = 0;
    for (int j = 0; j < colunasTotais; j++) {
        soma += matrix[linhaDaMatriz][j];
    }
    return soma / (double)colunasTotais;
}

double calculaMediaGeometrica(int colunaDaMatriz, int linhasTotais, int** matrix){
    double produto = 1;
    for (int i = 0; i < linhasTotais; i++) {
        produto *= matrix[i][colunaDaMatriz];
    }
    return pow(produto, 1.0 / linhasTotais);
}

void* calculaMedias (void* args){
    threadData* data = (threadData*) args;
    for (int i = data->comecoLinhaAritmetica; i <= data->finalLinhaAritmetica; i++) {
        data->mediasAritmetica[i] = calculaMediaAritmetica(i, *data->TotalColunas, data->matrix);
    }
    for (int i = data->comecoColunaGeometrica; i <= data->finalColunaGeometrica; i++) {
        data->mediasGeometrica[i] = calculaMediaGeometrica(i, *data->TotalLinhas, data->matrix);
    }
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc != 3)
    {
        printf("argumentos inválidos, ./ex2 <matriz> <arquivo de resultados>");
        return 1;
    }
    

    double time_spent = 0.0;
    clock_t begin = clock();
    int r, c, NrThreads, linhaPorThread, colunaPorThread, restoPorLinha, restoPorColuna; 
    int **matrix = read_matrix_from_file(argv[1], &r, &c);
    print_matrix(matrix, r, c);
    
    printf("Me diga quantas threads deseja criar, lembrando que o máximo é o número de linhas da matriz! que no caso é: %d\n", r);
    scanf("%d", &NrThreads);

    if (NrThreads > r){
        printf("Número de threads maior que o número de linhas da matriz, por favor insira um número menor ou igual a %d\n", r);
        return 1;
    }

    pthread_t threads[NrThreads];
    linhaPorThread = r / NrThreads;
    colunaPorThread = c / NrThreads;
    restoPorLinha = r % NrThreads;
    restoPorColuna = c % NrThreads;
    double mediasAritmetica[r];
    double mediasGeometrica[c];
    threadData DataDasThreads[NrThreads];

    // Cria Threads para realizar o cálculo
    for (int i = 0; i < NrThreads; i++) {   
        DataDasThreads[i].threadId = i;
        DataDasThreads[i].matrix = matrix;
        DataDasThreads[i].TotalColunas = &c;
        DataDasThreads[i].TotalLinhas = &r;
        DataDasThreads[i].comecoLinhaAritmetica = i * linhaPorThread;
        DataDasThreads[i].finalLinhaAritmetica = i != NrThreads - 1 ? DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread - 1 : DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread + restoPorLinha - 1;
        DataDasThreads[i].comecoColunaGeometrica = i * colunaPorThread;
        DataDasThreads[i].finalColunaGeometrica = i != NrThreads - 1 ? DataDasThreads[i].comecoColunaGeometrica + colunaPorThread - 1 : DataDasThreads[i].comecoColunaGeometrica + colunaPorThread + restoPorColuna - 1;
        DataDasThreads[i].mediasAritmetica = mediasAritmetica;
        DataDasThreads[i].mediasGeometrica = mediasGeometrica;
        int status = pthread_create(&threads[i], NULL, calculaMedias, (void*) &DataDasThreads[i]);
        if (status != 0){
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }
    
    for (int i = 0; i < NrThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Abre o arquivo para escrita
    FILE *file = fopen(argv[2], "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para escrita\n");
        return 1;
    }

    // Escreve as médias aritméticas no arquivo
    for (int i = 0; i < r; i++) {
        fprintf(file, "Média aritmética da linha %d: %f\n", i, mediasAritmetica[i]);
    }

    // Escreve as médias geométricas no arquivo
    for (int i = 0; i < c; i++) {
        fprintf(file, "Média geométrica da coluna %d: %f\n", i, mediasGeometrica[i]);
    }

    // Fecha o arquivo
    fclose(file);

    // Libera a memória alocada para a matriz
    for (int i = 0; i < r; i++) {
        free(matrix[i]);
    }
    free(matrix);

    clock_t end = clock();
 
    // calcula o tempo decorrido encontrando a diferença (end - begin) e
    // dividindo a diferença por CLOCKS_PER_SEC para converter em segundos
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
    printf("The elapsed time is %f seconds", time_spent);

    return 0;
}