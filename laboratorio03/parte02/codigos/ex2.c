#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include "matriz.h"

void* rotina (void* args){
    printf("Thread criada com sucesso\n");
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]){
    //pthread_t thread;
    //pthread_create(&thread, NULL, rotina, NULL);
    //pthread_join(thread, NULL);
    int r,c;
    int **matrix = read_matrix_from_file("matriz_6por8.in", &r, &c);
    print_matrix(matrix, r, c);
    
    return 0;
}