#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    Cabeçalho do código ex2.c
    Criado por: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 03/11/2024
    Finalidade: algoritmo que cria um número de threads pings e pongs a partir do usuário e pinga e ponga infinitamente;
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int pingpong = 0; // 0 = ping, 1 = pong

// Função para a thread ping
void* thread_ping(void* arg) {
    int idThread = *((int*)arg);

    while (1) {
        pthread_mutex_lock(&mutex);
        if (pingpong == 0) {
            printf("Eu, a thread ping nr %d vos digo: ping\n", idThread);
            sleep(1);
            pingpong = 1;
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(EXIT_SUCCESS);
}

// Função para a thread pong
void* thread_pong(void* arg) {
    int idThread = *((int*)arg);

    while (1) {
        pthread_mutex_lock(&mutex);

        if (pingpong == 1) {
            printf("Eu, a thread pong nr %d vos digo: pong\n", idThread);
            sleep(1);
            pingpong = 0;
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(EXIT_SUCCESS);
}

int main() {
    int num_ping, num_pong;

    printf("Digite o número de threads ping: ");
    scanf("%d", &num_ping);

    printf("Digite o número de threads pong: ");
    scanf("%d", &num_pong);

    pthread_t pings[num_ping];
    pthread_t pongs[num_pong];
    int pingIds[num_ping];
    int pongIds[num_pong];

    // criando as threads ping
    for (int i = 0; i < num_ping; i++) {
        pingIds[i] = i;
        pthread_create(&pings[i], NULL, thread_ping, (void*)&pingIds[i]);
    }

    // criando as threads pong
    for (int i = 0; i < num_pong; i++) {
        pongIds[i] = i;
        pthread_create(&pongs[i], NULL, thread_pong, (void*)&pongIds[i]);
    }

    // esperando as threads pings acabarem
    for (int i = 0; i < num_ping; i++) {
        pthread_join(pings[i], NULL);
    }

    // esperando as threads pongs acabarem
    for (int i = 0; i < num_pong; i++) {
        pthread_join(pongs[i], NULL);
    }

    // destroi o mutex caso o programa termine (ele nunca termina).
    pthread_mutex_destroy(&mutex);

    return 0;
}
