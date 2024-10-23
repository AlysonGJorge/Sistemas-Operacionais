#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>


#define tamanhoMaximoEntrada 1024
#define maximoArgumentos 100

int main(){
    printf("Bem vindo ao melhor Shell do mundo!!!!!\n");
    printf("Se você digitar 9 o programa fecha\n");

    char buffer[tamanhoMaximoEntrada];
    char* argumentos[maximoArgumentos];
    bool deFundo;

    while (1)
    {
        deFundo = false;
        printf("Digite um comando!!>");
        fflush(stdout); // limpa buffer de lixo de memória

        fgets(buffer, tamanhoMaximoEntrada, stdin);

        if (buffer == NULL)
        {
            break;
        }
        
        if (strcmp(buffer, "9") == 0)
        {
            break;
        }

        if (strcmp(buffer, "\n") == 0)
        {
            continue;;
        }
        
        char *token = strtok(buffer, " ");
        int numeroArgumentos = 0;

        while (token != NULL)
        {
            argumentos[numeroArgumentos] = token;
            printf("rapaz, olha como ta a situação %s", argumentos[numeroArgumentos]);
            token = strtok(NULL, " "); // vai pro próximo token
        }
        

        if (numeroArgumentos > 1 && strcmp(argumentos[numeroArgumentos-1], "&") == 0)
        {
            deFundo = true;
            argumentos[numeroArgumentos-1] = NULL; 
        }
        
        pid_t processo = fork();
            if (processo == 0)
            {
                // é filho
                printf("tem que comprar fralda \n");
                printf("%s", argumentos[0]);
                int CommandoValido = execvp(argumentos[0], &argumentos[0]);
                if (CommandoValido == -1)
                {
                    printf("Comando inválido");
                }
                exit(0);
            } else{
                // é pai
                printf("%d\n", deFundo);
                if (deFundo == false)
                {
                    printf("vou aguardar meu fiote");
                    waitpid(processo, NULL, 0);
                    printf("processo %d executado com sucesso.\n");
                } else {
                    printf("processo %d executando seu comando em segundo plano\n", processo);
                    deFundo = false;
                } 
            }
            

    }
    return 0;
}