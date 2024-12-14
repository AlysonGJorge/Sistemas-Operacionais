
/*
    Cabeçalho do código leitores/escritores
    Criado por: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 12/12/2024
    Finalidade: algoritmo dos leitores e escritores onde existem dois tipos de threads:
    Leitores: apenas leem o recurso compartilhado e não causam alterações.
    Escritores: modificam o recurso compartilhado, potencialmente alterando a informação disponível para os leitores
    Implementação visa garantir que:
    Segurança: Escritores tenham acesso exclusivo ao recurso (não podem haver leitores ou outros escritores enquanto um escritor está ativo).
    Eficiência: Vários leitores podem acessar o recurso simultaneamente;
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

pthread_mutex_t mutex;
pthread_cond_t cond_readers;
pthread_cond_t cond_writers;

int readers_count = 0;
int writers_count = 0;
int waiting_writers = 0;

void *reader(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        // Esperar enquanto há escritores ativos
        while (writers_count > 0) {
            pthread_cond_wait(&cond_readers, &mutex);
        }

        // Incrementa o contador de leitores ativos
        readers_count++;
        printf("Leitor %d está lendo. Leitores ativos: %d\n", id, readers_count);

        pthread_mutex_unlock(&mutex);

        // Simula a leitura
        sleep(rand() % 3 + 1);

        pthread_mutex_lock(&mutex);

        // Decrementa o contador de leitores ativos
        readers_count--;
        printf("Leitor %d terminou de ler. Leitores ativos: %d\n", id, readers_count);

        // Se não há mais leitores, sinaliza escritores
        if (readers_count == 0) {
            pthread_cond_signal(&cond_writers);
        }

        pthread_mutex_unlock(&mutex);

        // Simula intervalo entre leituras
        sleep(rand() % 3 + 1);
    }

    free(arg);
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        // Incrementa escritores esperando
        waiting_writers++;

        // Esperar enquanto há leitores ou escritores ativos
        while (readers_count > 0 || writers_count > 0) {
            pthread_cond_wait(&cond_writers, &mutex);
        }

        // Escritor entra
        waiting_writers--;
        writers_count++;
        printf("Escritor %d está escrevendo.\n", id);

        pthread_mutex_unlock(&mutex);

        // Simula a escrita
        sleep(rand() % 3 + 1);

        pthread_mutex_lock(&mutex);

        // Escritor termina
        writers_count--;
        printf("Escritor %d terminou de escrever.\n", id);

        // Prioriza escritores esperando
        if (waiting_writers > 0) {
            pthread_cond_signal(&cond_writers);
        } else {
            pthread_cond_broadcast(&cond_readers);
        }

        pthread_mutex_unlock(&mutex);

        // Simula intervalo entre escritas
        sleep(rand() % 3 + 1);
    }

    free(arg);
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_readers, NULL);
    pthread_cond_init(&cond_writers, NULL);

    srand(time(NULL));

    // Cria threads leitoras
    for (int i = 0; i < NUM_READERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers[i], NULL, reader, id);
    }

    // Cria threads escritoras
    for (int i = 0; i < NUM_WRITERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers[i], NULL, writer, id);
    }

    // Espera as threads (nunca vão terminar neste caso)
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_readers);
    pthread_cond_destroy(&cond_writers);

    return 0;
}
