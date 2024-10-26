#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

/*
    Cabeçalho do código ex1.c
    Criador por: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 26/10/2024
    Finalidade: Algoritmo que cria de forma recursiva uma árvore de processos com base no valor informado;
*/

/*
    parametros da função criaProcessosReborn:
        processoAtual: inteiro que representa o processo atual
        nivelProcesos: inteiro que representa o limite de processos que podem ser criados */
void criaProcessosReborn (int processoAtual, int nivelProcesos) {
    if (processoAtual == nivelProcesos) {
        return ;
    }

   for (int i = 0; i < 2; i++)
   {
        pid_t pid = fork();


        if (pid < 0) // Criação do processo falhou
        {
            fprintf(stderr, "Erro ao criar processo filho.\n");
            exit(1);
        }
        else
        if (pid == 0) // processo filho
        {
            printf("Processo %d criado pelo processo pai %d).\n",  getpid(), getppid());
            criaProcessosReborn(processoAtual + 1, nivelProcesos);
            getchar();
            exit(0);
        }
   }
    for (int i = 0; i < 2; i++)wait(NULL); // espera pelos filhos
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // condicional que verifica se o número de argumentos é diferente de 2 (nome do programa + número de niveis)
        fprintf(stderr, "Uso: %s <numero_de_niveis>\n", argv[0]);
        return 1;
    }
    
    long const int processosMaximos =sysconf(_SC_CHILD_MAX); // verifica o número máximo de processos que podem ser criados
    int nivelProcesos = atoi(argv[1]); // Resgata o valor informado de processos máximos pelo usuário
    int limiteProcessos = (int)pow(2,nivelProcesos); // Calcula o limite de processos que serão criados pelo programa;

    if(processosMaximos < limiteProcessos){
        printf("o número de processos que você deseja (%d) excede o limite (%d)", nivelProcesos, processosMaximos);
        return 1;
    }

    if (nivelProcesos < 1) {
        fprintf(stderr, "O número de níveis deve ser maior ou igual a 1.\n");
        return 1;
    }

    // Imprime o PID do processo raiz
    printf("Processo raiz (pid: %d).\n", getpid());
    criaProcessosReborn(1,nivelProcesos);
    getchar();
    return 0;
}
