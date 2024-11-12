#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "matriz.h"
#include <time.h>

/**
 * cabeçalho do ex2.c
 * 
 * Autores: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
 * Data: 23/10/2024
 * Finalidade: utilizar Threads para realizar cálculos de médias aritméticas e geométricas em uma matriz
 */

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


/*
    parametros da função calculaMediaAritmetica:
        linhaDaMatriz: número da linha que será calculada a média
        colunasTotais: número totais de colunas que cada linha percorre
        matrix: matriz que será utilizada para calcular a média
*/
double calculaMediaAritmetica(int linhaDaMatriz, int colunasTotais, int** matrix){
    double soma = 0;
    for (int j = 0; j < colunasTotais; j++) {
        soma += matrix[linhaDaMatriz][j];
    }
    return soma / (double)colunasTotais;
}

/*
    parametros da função calculaMediaGeometrica:
        colunaDaMatriz: número da coluna que será calculada a média
        linhasTotais: número totais de linhas que cada coluna percorre
        matrix: matriz que será utilizada para calcular a média geométrica
*/
long double calculaMediaGeometrica(int colunaDaMatriz, int linhasTotais, int** matrix){
    long double produto = 1;
    for (int i = 0; i < linhasTotais; i++) {
        produto *= matrix[i][colunaDaMatriz];
    }

    long double resultado = pow(produto, 1.0 / linhasTotais);
    return resultado;
}

/*
    parametros da função calculaMediaGeometrica:
        args: estrutura que contém os dados necessários para calcular as médias
*/
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

    // Verifica se o usuário passou os argumentos corretamente
    if (argc > 3 || argc < 2)
    {
        printf("argumentos inválidos\n");
        printf("caso queira que o programa leia uma matriz advinda de um arquivo incialize com %s <arquivo saida> <matrizEntrada>\n", argv[0]);
        printf("caso queira que o programa gere uma matriz aleatória incialize apenas com: %s <arquivo saida>\n", argv[0]);
        return 1;
    }
    
    double time_spent = 0.0;
    // Inicia a contagem do tempo
    clock_t begin = clock();
    int r, c, NrThreads, linhaPorThread, colunaPorThread, restoPorLinha, restoPorColuna, maximoThreads; 
    int **matrix;
    FILE *fileEntrada;

    // Caso o usuário deseje ler uma matriz de um arquivo ele deve informar o nome do arquivo
    // Após isso a matriz é lida e impressa na tela
    if (argc == 3)
    {
    matrix = read_matrix_from_file(argv[2], &r, &c);
    print_matrix(matrix, r, c);
    }


    // Caso o usuário deseje criar uma matriz aleatória

    if (argc == 2)
    {
    printf("informe o número de linhas da matriz\n");
    scanf("%d", &r);
    printf("informe o número de colunas da matriz\n");
    scanf("%d", &c);

    // gera a matriz aleatória
    matrix = create_matrix(r, c);
    generate_elements(matrix, r, c, 100);

    // cria o nome do arquivo
    char nomedoarquivo[200];
    sprintf(nomedoarquivo, "matriz_%dpor%d.in", r, c);

    // abre o arquivo para escrita
    fileEntrada = fopen(nomedoarquivo, "w");
    if (fileEntrada == NULL) {
        printf("Erro ao abrir o arquivo para escrita\n");
        return 1;
    }
    print_matrix(matrix, r, c);

    char primeiraLinha[10];
    
    // coloca na primeira linha do arquivo o identificador
    snprintf(primeiraLinha, sizeof(primeiraLinha), "%dx%d\n", r, c);

    // preenche o arquivo com os dados da matriz
    fprintf(fileEntrada, primeiraLinha);
    for (int i = 0; i < r; i++)
    {
        
            for (int j = 0; j < c; j++)
            {
                if (j == c - 1)
                {
                    fprintf(fileEntrada, "%d\n", matrix[i][j]);
                } else{
                    fprintf(fileEntrada, "%d ", matrix[i][j]);
                }
            }
        } 
    fclose(fileEntrada); // fecha o arquivo
    }


    // Verifica qual será o máximo de threads possivel no sistema
    if (r > c)
    {
        maximoThreads = c;
    } else{
        maximoThreads = r;
    }

    printf("Me diga quantas threads deseja criar, lembrando que o máximo é o menor número entre colunaXLinha! que no caso é: %d\n", maximoThreads);
    scanf("%d", &NrThreads);

    if (NrThreads > maximoThreads){
        printf("Número de threads maior que o menor número entre colunaXlinha, por favor insira um número menor ou igual a %d\n", maximoThreads);
        return 1;
    }

    pthread_t threads[NrThreads]; // cria um vetor de threads
    linhaPorThread = r / NrThreads; // aritmetico
    colunaPorThread = c / NrThreads; // geometrico
    restoPorLinha = r % NrThreads; // aritmetico
    restoPorColuna = c % NrThreads; // geometrico
    double mediasAritmetica[r];
    long double mediasGeometrica[c];
    threadData DataDasThreads[NrThreads];

    // Cria Threads para realizar o cálculo, faz os cálculos necessários para determinal qual será o caminho de cada thread para realizar
    // Ambos os cálculos de média

    for (int i = 0; i < NrThreads; i++) {   
        DataDasThreads[i].threadId = i; // passa o id da thread
        DataDasThreads[i].matrix = matrix; // passa a matriz para a thread
        DataDasThreads[i].TotalColunas = &c; // usando o endereço para não consumir muita memória
        DataDasThreads[i].TotalLinhas = &r; // usando o endereço para não consumir muita memória
        DataDasThreads[i].comecoLinhaAritmetica = i * linhaPorThread; // calcula o começo da linha aritmetica
        // calcula qual será o final da linha aritmetica
        // verifica se é a última thread, se for adiciona o resto da divisão para a última thread
        DataDasThreads[i].finalLinhaAritmetica = i != NrThreads - 1 ? DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread - 1 : DataDasThreads[i].comecoLinhaAritmetica + linhaPorThread + restoPorLinha - 1;
        DataDasThreads[i].comecoColunaGeometrica = i * colunaPorThread; // calcula o começo da coluna geometrica
        // calcula qual será o final da coluna geometrica
        // verifica se é a última thread, se for adiciona o resto da divisão para a última thread
        DataDasThreads[i].finalColunaGeometrica = i != NrThreads - 1 ? DataDasThreads[i].comecoColunaGeometrica + colunaPorThread - 1 : DataDasThreads[i].comecoColunaGeometrica + colunaPorThread + restoPorColuna - 1;
        DataDasThreads[i].mediasAritmetica = mediasAritmetica; // passa o endereço da média aritmetica
        DataDasThreads[i].mediasGeometrica = mediasGeometrica; // passa o endereço da média geometrica
        int status = pthread_create(&threads[i], NULL, calculaMedias, (void*) &DataDasThreads[i]);
        if (status != 0){
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }

    // Espera as threads terminarem
    for (int i = 0; i < NrThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Abre o arquivo de saida para escrita
    FILE *file = fopen(argv[1], "w");
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
        fprintf(file, "Média geométrica da coluna %d: %Lf\n", i, mediasGeometrica[i]);
    }

    // Fecha o arquivo
    fclose(file);

    // Libera a memória alocada para a matriz
    for (int i = 0; i < r; i++) {
        free(matrix[i]);
    }
    free(matrix);

    // Finaliza a contagem do tempo
    clock_t end = clock();
 
    // calcula o tempo decorrido
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
 
    printf("Resultado gerado com sucessos!");
    printf("Demorou %f segundos\n", time_spent);

    return 0;
}
