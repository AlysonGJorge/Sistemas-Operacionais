#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Uso: %s <numero_de_niveis>\n", argv[0]);
        return 1;
    }

    pid_t pid=fork();

    if(pid < 0 ){
        //É erro
        printf("deu ruim fml");
        return 0;
    }
     else if(pid == 0){
        //Processo filho
        int deubom = execvp(argv[1], &argv[1]);
        if(!deubom){
            printf("Erro, os comando enviados não form reconhecidos como comando do bash Linux\n");
            return 0;
        }
    }else {
        //Processo pai
        int* status;
        printf("I am your father...\n");
        waitpid(pid, status, 0);
        printf("The Force is strong with this one. \n");
    }
    return 0;
}