#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    int *vetor;
    int threadId;

    int comecoBusca;
    int finalBusca;

    int* valorAProcurar;

} threadData;


void* procuraNoVetor (void* args){
    threadData* data = (threadData*) args;
    int encontrado = 0;

    printf("eu sou a thread %d e estou procurando o valor %d nas posições %d até %d\n", data->threadId, *data->valorAProcurar, data->comecoBusca, data->finalBusca);    
    for (int i = data->comecoBusca; i < data->finalBusca; i++)
    {
        if(data->vetor[i] == *data->valorAProcurar){
            printf("eu, a thread %d encontrei o valor %d na posição %d\n",data->threadId, *data->valorAProcurar, i);
            encontrado = 1;
        }
    }

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

    if (NrThreads > tamanhoVetor){
        printf("Número de threads maior que o tamanho do vetor, por favor insira um número menor ou igual a %d\n", tamanhoVetor);
        return 1;
    }

    printf("me diga qual o valor que você quer que eu busque\n");
    scanf("%d", &vlSolicitado);

    pthread_t threads[NrThreads];
    buscaPorThread = tamanhoVetor / NrThreads;
    restoBusca = tamanhoVetor % NrThreads;
    threadData DataDasThreads[NrThreads];

    // Cria Threads para realizar o cálculo
    for (int i = 0; i < NrThreads; i++) {  
        DataDasThreads[i].threadId = i;
        DataDasThreads[i].vetor = vetorAleatorio;
        DataDasThreads[i].comecoBusca = i * buscaPorThread;
        DataDasThreads[i].valorAProcurar = &vlSolicitado;
        DataDasThreads[i].finalBusca = i != NrThreads - 1 ? DataDasThreads[i].comecoBusca + buscaPorThread - 1 : DataDasThreads[i].comecoBusca + buscaPorThread + restoBusca - 1;
        int status = pthread_create(&threads[i], NULL, procuraNoVetor, (void*) &DataDasThreads[i]);
        if (status != 0){
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }
    
    void *status;
    int encontrado = 0;
    for (int i = 0; i < NrThreads; i++) {
        pthread_join(threads[i], &status);
        if (status == EXIT_SUCCESS) {
            encontrado = 1;
        }
    }

    if (!encontrado)
    {
        printf("\nO número %d não foi encontrado no vetor\n", vlSolicitado);
    }


    return 0;
}