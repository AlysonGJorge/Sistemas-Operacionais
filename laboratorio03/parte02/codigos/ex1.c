#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


/**
 * cabeçalho do ex1.c
 * 
 * Autores: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
 * Data: 23/10/2024
 * Finalidade: utilizar Threads para realizar uma busca dentro de um vetor aleatório
 */

typedef struct {
    int *vetor;
    int threadId;

    int comecoBusca;
    int finalBusca;

    int* valorAProcurar;

} threadData;



/*
    parametros da função procuraNoVetor:
        args: estrutura que contém os dados necessários para realizar a busca 
*/

void* procuraNoVetor (void* args){
    threadData* data = (threadData*) args;
    int encontrado = 0;

    printf("eu sou a thread %d e estou procurando o valor %d nas posições %d até %d\n", data->threadId, *data->valorAProcurar, data->comecoBusca, data->finalBusca);    
    // procura o valor no vetor
    for (int i = data->comecoBusca; i <= data->finalBusca; i++)
    {
        if(data->vetor[i] == *data->valorAProcurar){
            printf("eu, a thread %d encontrei o valor %d na posição %d\n",data->threadId, *data->valorAProcurar, i);
            encontrado = 1;
        }
    }

    // verifica se foi encontrado ou não o valor
    if (!encontrado)
    {
        pthread_exit((void*)EXIT_FAILURE);
    }

    pthread_exit((void*)EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    int tamanhoVetor, NrThreads, restoBusca, buscaPorThread, vlSolicitado;

    printf("informe o tamanho do vetor\n");
    scanf("%d", &tamanhoVetor);
    srand(time(NULL));

    // gera um vetor aleatório

    int vetorAleatorio[tamanhoVetor];
    for (int i = 0; i < tamanhoVetor; i++)
    {
        vetorAleatorio[i] = rand() % 100;
        printf("%d ", vetorAleatorio[i]);
        if ( i == tamanhoVetor - 1)
        {
            printf("\n");
        }
    }
    
    printf("Me diga quantas threads deseja criar, lembrando que o máximo é o tamanho do vetor! que no caso é: %d\n", tamanhoVetor);
    scanf("%d", &NrThreads);

    // verifica se o número de threads é maior que o tamanho do vetor
    if (NrThreads > tamanhoVetor){
        printf("Número de threads maior que o tamanho do vetor, por favor insira um número menor ou igual a %d\n", tamanhoVetor);
        return 1;
    }

    printf("me diga qual o valor que você quer que eu busque\n");
    scanf("%d", &vlSolicitado);


    // prepara para a inicialização das threads
    pthread_t threads[NrThreads];
    buscaPorThread = tamanhoVetor / NrThreads;
    restoBusca = tamanhoVetor % NrThreads;
    threadData DataDasThreads[NrThreads];

    // Cria Threads para realizar o cálculo
    for (int i = 0; i < NrThreads; i++) {  
        DataDasThreads[i].threadId = i; // passa o id da thread
        DataDasThreads[i].vetor = vetorAleatorio; // passa o vetor
        DataDasThreads[i].comecoBusca = i * buscaPorThread; // calcula o começo da busca
        DataDasThreads[i].valorAProcurar = &vlSolicitado; // passa o valor a ser procurado
        // calcula qual será o final da busca
        // verifica se é a última thread, se for adiciona o resto da divisão para a última thread
        DataDasThreads[i].finalBusca = i != NrThreads - 1 ? DataDasThreads[i].comecoBusca + buscaPorThread - 1 : DataDasThreads[i].comecoBusca + buscaPorThread + restoBusca - 1; 
        int status = pthread_create(&threads[i], NULL, procuraNoVetor, (void*) &DataDasThreads[i]);
        if (status != 0){
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }
    
    void *status;
    int encontrado = 0;
    // espera as threads terminarem
    for (int i = 0; i < NrThreads; i++) {
        pthread_join(threads[i], &status);
        if (status == EXIT_SUCCESS) {
            encontrado = 1;
        }
    }


    // verifica se o valor foi encontrado ou não
    if (!encontrado)
    {
        printf("\nO número %d não foi encontrado no vetor\n", vlSolicitado);
    } else
    {
        printf("\nO número %d foi encontrado no vetor\n", vlSolicitado);
    }

    return 0;
}