#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <math.h>
#include "matriz.h"

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
    long double* mediasGeometrica;

} threadData;

double calculaMediaAritmetica(int linhaDaMatriz, int colunasTotais, int** matrix){
    double soma = 0;


        for (int j = 0; j < colunasTotais; j++)
        {

            soma += (double)matrix[linhaDaMatriz][j];
        }

    return soma/colunasTotais;
}

long double calculaMediaGeometrica(int colunaDaMatriz, int linhasTotais, int** matrix){
    printf("dentro do calculaMediaGeometrica\n");
    int produto = 1;
    printf("colunaDaMatriz: %d\n", colunaDaMatriz);
    printf("linhasTotais: %d\n", linhasTotais);

    for (int i = 0; i < linhasTotais; i++)
    {
        produto *= matrix[i][colunaDaMatriz];
        printf("produto: %d\n", produto);
    }
    double blablabla = 1.0/ linhasTotais;
    long double resultado = pow(produto, blablabla);
    printf("resultado: %d\n", resultado);
    return resultado;
}

void* calculaMedias (void* args){
    threadData* data = (threadData*) args;

    for (int i = data->comecoLinhaAritmetica; i <= data->finalLinhaAritmetica; i++)
    {
        data->mediasAritmetica[i] = calculaMediaAritmetica(i, *data->TotalColunas, data->matrix);
        printf("\n");
    }

    printf("tô querendo entrar no for \n");
    printf("%d\n", data->comecoColunaGeometrica);
    printf("%d\n", data->finalColunaGeometrica);
    printf("%d\n", *data->TotalLinhas);
    for (int i = data->comecoColunaGeometrica; i < data->finalColunaGeometrica; i++)
    {
        printf("%dº geometrica\n", i);
        data->mediasGeometrica[i] = calculaMediaGeometrica(i, *data->TotalLinhas, data->matrix);
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]){
    int r,c, NrThreads, linhaPorThread, colunaPorThread, restoPorLinha, restoPorColuna; 
    //pthread_t thread;

    //pthread_join(thread, NULL);
    int **matrix = read_matrix_from_file("matriz_6por8.in", &r, &c);
    print_matrix(matrix, r, c);
    
    printf("Me diga quantas threads deseja criar, lembrando que o máximo é o número de linhas da matriz! que no caso é: %d", r);
    scanf("%d", &NrThreads);

    if (NrThreads > r){
        printf("Número de threads maior que o número de linhas da matriz, por favor insira um número menor ou igual a %d", r);
        return 1;
    }
    pthread_t threads[NrThreads];
    linhaPorThread = r/NrThreads;

    colunaPorThread = c/NrThreads;

    restoPorLinha = r%NrThreads;

    restoPorColuna = c%NrThreads;
    double mediasAritmetica[r];
    long double mediasGeometrica[c];
    threadData DataDasThreads[NrThreads];
    // Cria Threads para realizar o cálculo
    for (int i = 0; i < NrThreads; i++)
    {   
        DataDasThreads[i].threadId = i; // identificador de qual thread é para saber em qual posição do vetor de médias deve inserir o resultado;
        DataDasThreads[i].matrix = matrix; // endereço da matrix para realizar a busca 
        DataDasThreads[i].TotalColunas = &c ; // passado referência para não consumir memória com valor repetido
        DataDasThreads[i].TotalLinhas = &r; // passado referência para não consumir memória com valor repetido
        DataDasThreads[i].comecoLinhaAritmetica = i*linhaPorThread; // calcula o começo da linha aritmética
        DataDasThreads[i].finalLinhaAritmetica = i != NrThreads-1 ? DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread - 1 : DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread + restoPorLinha - 1; // calcula o final da linha aritmética
        DataDasThreads[i].comecoColunaGeometrica = i*colunaPorThread; // calcula o começo da coluna aritmética
        DataDasThreads[i].finalColunaGeometrica = i != NrThreads-1 ? DataDasThreads[i].comecoColunaGeometrica + colunaPorThread - 1 : DataDasThreads[i].comecoColunaGeometrica+colunaPorThread + restoPorColuna - 1; // calcula o final da coluna aritmética
        DataDasThreads[i].mediasAritmetica = mediasAritmetica; // endereço do vetor de médias aritméticas
        DataDasThreads[i].mediasGeometrica = mediasGeometrica; // endereço do vetor de médias geométricas
        int status = pthread_create(&threads[i], NULL, calculaMedias, (void*) &DataDasThreads[i]);
        if (status != 0){
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }
    
    for (int i = 0; i < NrThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    for (int i = 0; i < r; i++)
    {
        printf("Média aritmética da linha %d: %d\n", i, mediasAritmetica[i]);
    }

        for (int i = 0; i < c; i++)
    {
        printf("Média geométrica da linha %d: %d\n", i, mediasGeometrica[i]);
    }
    
    return 0;
}