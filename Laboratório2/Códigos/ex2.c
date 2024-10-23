#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Essa função é a responsável 
// parametros: limitePro
void criaprocessos(int limiteProcessos) {
    if (limiteProcessos <= 1) {
        return;
    }

    pid_t leftson = fork();


    if (leftson < 0) {
        // Erro na criação do processo filho esquerdo
        perror("Erro na criação do filho esquerdo");
    } else if (leftson == 0) {
        // Processo filho esquerdo
        printf("Processo filho esquerdo criado (pid: %d, pai: %d)\n", getpid(), getppid());
        criaprocessos(limiteProcessos - 1);
        exit(0); // O filho termina aqui
    } else {
        int status;
        // Processo pai aguarda o filho esquerdo terminar
        waitpid(leftson, &status, 0);
    }

    pid_t rightson = fork();

    if (rightson < 0) {
        perror("Erro na criação do filho direito");
        return;
    } else if (rightson == 0) {
        // Processo filho direito
        printf("Processo filho direito criado (pid: %d, pai: %d)\n", getpid(), getppid());
        criaprocessos(limiteProcessos - 1);
        exit(0); // O filho termina aqui
    } else {
        int status;
        // Processo pai aguarda o filho direito terminar
        waitpid(rightson, &status, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_de_niveis>\n", argv[0]);
        return 1;
    }
    
    long const int processosMaximos = sysconf(_SC_CHILD_MAX);
    int limiteProcessos = atoi(argv[1]);

    if(processosMaximos < limiteProcessos){
        printf("o número de processos que você deseja (%d) excede o limite (%d)", limiteProcessos, processosMaximos);
        return 1;
    }

    if (limiteProcessos < 1) {
        fprintf(stderr, "O número de níveis deve ser maior ou igual a 1.\n");
        return 1;
    }

    // Imprime o PID do processo raiz
    printf("Processo raiz (pid: %d).\n", getpid());
    criaprocessos(limiteProcessos);
    return 0;
}
