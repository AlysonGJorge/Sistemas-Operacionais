#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

/*
    Cabeçalho do código ex1.c
    Criador por: Alyson Gonçalves J1orge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 16/10/2024
    Finalidade: esse código tem como finalidade criar um número determinado pelo usuário de processos que irão procurar em um
    vetor de números aleatórios um número também determinado pelo usuário, caso nenhum número for achado o processo pai deve retornar uma mensagem
    mensagem de não encontrado
*/

/*
    função procuraDentroDoVetor
    parâmetros:
        vetorAleatorio: vetor de números aleatórios
        comeco: posição inicial do vetor que o processo filho irá procurar
        fim: posição final do vetor que o processo filho irá procurar
        vlrProcurado: valor que o usuário deseja procurar
        nrFilho: número do filho que está realizando a busca
    finalidade: essa função tem como finalidade procurar um número dentro de um vetor de números aleatórios a partir de um ponto inicial e final
*/
int procuraDentroDoVetor(int vetorAleatorio[], int comeco, int fim, int vlrProcurado, int nrFilho){
    bool achei = false;

    for (int i = comeco; i < fim; i++){
        if(vetorAleatorio[i]== vlrProcurado){
            achei = true;
            printf("Eu, o %dº filho com o PID %d fui de %d até %d e encontrei o valor %d na posição %d\n", nrFilho, getpid(), comeco,fim,vlrProcurado,i);
        }
    }
    if (!achei)
    {
        printf("Eu, o %dº com o PID %d fui de %d até %d e não encontrei o número %d :(\n", nrFilho, getpid(), comeco, fim, vlrProcurado);
        return 0;
    }
    return 1;
}

int main(){
    int tamanhoVetor, qtdFilhos, vlSolicitado,qtdDivisao,restoDaDivisao, encontrado = false;
    
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
            int comeco = (i == 0) ? 0 : i * qtdDivisao;
            int fim = (i == qtdFilhos - 1) ? (comeco + qtdDivisao + restoDaDivisao)-1 : (comeco + qtdDivisao) -1;
            int indexDoNumeroAchado;
            if (procuraDentroDoVetor(&vetorAleatorio[0],comeco,fim,vlSolicitado, i) == 1)
            {
                exit(EXIT_SUCCESS);
            }
            exit(EXIT_FAILURE);
        }
    }

    int status;
    
    for (int i = 0; i < qtdFilhos; i++)
    {
        waitpid(filhos[i], &status, 0);
        if (status == EXIT_SUCCESS) {
            encontrado = true;
        }
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
    
    if (!encontrado)
    {
        printf("\nO número %d não foi encontrado no vetor\n", vlSolicitado);
    }

    return 0;
}