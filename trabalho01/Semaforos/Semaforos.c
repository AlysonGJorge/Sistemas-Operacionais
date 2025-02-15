/*
 * FUNCIONALIDADE: Implementação de um sistema de avaliação de Padawans gerenciado por Yoda.
 * AUTORES: Natanael Aparecido Tagliaferro Galafassi, Alyson Gonçalves Jorge, Hudson Taylor Perrut Cassim.
 * DATA: 20/12/2024
 */

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <algorithm>

#define NUM_PADAWANS 5
#define NUM_ESPECTADORES 10

// Variáveis globais
int numPadawans = NUM_PADAWANS;
int numEspectadores = NUM_ESPECTADORES;
int totalPadawans;
int totalEspectadores;

std::list<int> padawansNoSalao;
std::list<int> espectadoresNoSalao;
std::list<int> padawansAprovados;
std::queue<int> filaPadawans;

bool salaoAberto = false;
bool testesConcluidos = false;

// Semáforos
sem_t semEntradaSalao;
sem_t semTeste;
sem_t semCorteTranca;
sem_t semYoda;

void cumprimentaMestresAvaliadores(int id) {
    std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;
}

bool estaAprovado(int id) {
    return std::find(padawansAprovados.begin(), padawansAprovados.end(), id) != padawansAprovados.end();
}

// Funções principais
void liberarEntrada() {
    int tempo = rand() % 10 + 1;
    std::cout << "[Yoda]: Entrada liberada por " << tempo << " segundos" << std::endl;
    salaoAberto = true;

    for (int i = 0; i < numPadawans; ++i)
        sem_post(&semEntradaSalao);

    sleep(tempo);
    salaoAberto = false;
}

void *yodaThread(void *arg) {
    std::cout << "Yoda começou a gerenciar a cerimônia" << std::endl;

    while (totalPadawans > 0 && totalEspectadores > 0) {
        liberarEntrada();
        sem_wait(&semYoda); // Aguarda os testes serem concluídos
        std::cout << "[Yoda]: Resultados dos testes sendo anunciados..." << std::endl;

        for (auto &padawan : padawansNoSalao) {
            if (rand() % 100 >= 50) {
                padawansAprovados.push_back(padawan);
                std::cout << "Padawan_" << padawan << " foi aprovado!" << std::endl;
            }
        }

        for (auto &padawan : padawansAprovados) {
            sem_post(&semCorteTranca);
        }
    }
    std::cout << "[Yoda]: Cerimônia finalizada." << std::endl;
    pthread_exit(NULL);
}

void *padawanThread(void *arg) {
    int id = *(int *)arg;
    delete (int *)arg;

    sem_wait(&semEntradaSalao);
    if (salaoAberto) {
        padawansNoSalao.push_back(id);
        std::cout << "Padawan_" << id << " entrou no salão." << std::endl;

        cumprimentaMestresAvaliadores(id);
        sem_post(&semTeste);
    }

    sem_wait(&semTeste);
    std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;

    if (estaAprovado(id)) {
        sem_wait(&semCorteTranca);
        std::cout << "Padawan_" << id << " teve sua trança cortada." << std::endl;
    }

    std::cout << "Padawan_" << id << " saiu do salão." << std::endl;
    pthread_exit(NULL);
}

int main() {
    pthread_t yoda;
    pthread_t padawans[NUM_PADAWANS];

    sem_init(&semEntradaSalao, 0, 0);
    sem_init(&semTeste, 0, 0);
    sem_init(&semCorteTranca, 0, 0);
    sem_init(&semYoda, 0, 0);

    totalPadawans = NUM_PADAWANS;
    totalEspectadores = NUM_ESPECTADORES;

    pthread_create(&yoda, NULL, yodaThread, NULL);

    for (int i = 0; i < NUM_PADAWANS; i++) {
        int *id = new int(i);
        pthread_create(&padawans[i], NULL, padawanThread, id);
    }

    pthread_join(yoda, NULL);
    for (int i = 0; i < NUM_PADAWANS; i++) {
        pthread_join(padawans[i], NULL);
    }

    sem_destroy(&semEntradaSalao);
    sem_destroy(&semTeste);
    sem_destroy(&semCorteTranca);
    sem_destroy(&semYoda);

    return 0;
}