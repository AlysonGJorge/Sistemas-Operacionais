#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int procuraDentroDoVetor(int vetorAleatorio[], int comeco, int fim, int item, int* indexNumeroAchado){
    for (int i = comeco; i < fim; i++){
        if(vetorAleatorio[i]== item){
            *indexNumeroAchado = i;
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

    if (qtdFilhos > tamanhoVetor)
    {
        printf("ERRO, NÚMERO DE FILHOS NÃO PODE SER MAIOR QUE O NÚMERO DO VETOR DE BUSCA!");
        return 0;
    }
    
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
            int fim = (i == qtdFilhos - 1) ? comeco + qtdDivisao + restoDaDivisao : comeco + qtdDivisao;
            int indexDoNumeroAchado;
            if(procuraDentroDoVetor(&vetorAleatorio[0],comeco,fim,vlSolicitado, &indexDoNumeroAchado)){
                printf("Eu o %dº filho com o PID %d fui de %d até %d, achei o número %d na posição %d\n", i,getpid(), comeco, fim, vlSolicitado, indexDoNumeroAchado);
            } else{
                printf("Eu, o %dº filho com o PID %d fui de %d até %d e não encontrei o número desejado :(\n", i, getpid(), comeco, fim);
            };
            exit(0);
        }
        
    }
    for (int i = 0; i < qtdFilhos; i++)
    {
        waitpid(filhos[i], NULL, 0);
    }

    printf("o vetor continha esses números:\n");
    for (int i = 0; i < tamanhoVetor; i++)
    {
        printf("%d", vetorAleatorio[i]);
        if (i != tamanhoVetor-1)
        {
            printf(",");
        }
        
    }
    
    
    return 0;
}