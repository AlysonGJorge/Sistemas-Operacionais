#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int procuraDentroDoVetor(int vetorAleatorio[], int comeco, int fim, int item){
    for (int i = comeco; i < fim; i++){
        if(vetorAleatorio[i]== item){
            return 1;
        }
    }
    return 0;
}

int main(){
    int tamanhoVetor, qtdFilhos, vlSolicitado,qtdDivisao,restoDaDivisao;
    
    printf("informe o tamanho do vetor\n");
    scanf("%d", &tamanhoVetor);
    printf("me informe quantos processos filhos você quer para realizar a busca\n");
    scanf("%d", &qtdFilhos);
    printf("me diga qual o valor que você quer que eu busque\n");
    scanf("%d", &vlSolicitado);

    srand(time(NULL));
    pid_t filhos[qtdFilhos];
    int vetorAleatorio[tamanhoVetor];
    for (int i = 0; i < tamanhoVetor; i++)
    {
        vetorAleatorio[i] = rand() % 100;
    }
    qtdDivisao = tamanhoVetor/qtdFilhos;
    restoDaDivisao = tamanhoVetor%qtdFilhos;

    for (int i = 0; i < qtdFilhos; i++)
    {
        filhos[i] = fork();
        if (filhos[i] == 0){
            //é filho
            int comeco = i * qtdDivisao;
            int fim = (i == qtdFilhos - 1) ? restoDaDivisao : comeco + qtdDivisao;
            if(procuraDentroDoVEtor(&vetorAleatorio[0],comeco,fim,vlSolicitado)){
                printf("Eu o %dº filho com o PID %d achei o número %d\n", i,getpid(), vlSolicitado);
            } else{
                printf("Eu, o %dº filho não encontrei o número desejado :(\n", i);
            };
            exit(0);
        }
        
    }

    for (int i = 0; i < qtdDivisao; i++)
    {
        wait(NULL);
    }

    return 0;
}