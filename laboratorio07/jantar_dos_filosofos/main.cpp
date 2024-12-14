/*
 * FUNCIONALIDADE: Implementação do problema do Jantar dos Filósofos utilizando semáforos e threads.
 * AUTORES: Natanael Aparecido Tagliaferro Galafassi, Alyson Gonçalves Jorge, Hudson Taylor Perrut Cassim.
 * DATA: 13/12/2024
 */

#include <iostream>
#include <queue>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5 // Numero de filosofos.
#define LEFT (i+NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS // Numero do vizinho a esquerda de i.
#define RIGHT (i+1)%NUM_PHILOSOPHERS // Numero do vizinho a direita de i.

enum State {THINKING, HUNGRY, EATING}; // Estados dos filosofos.

typedef sem_t semaphore; // Definição de semáforo.

semaphore mutex = {1}; // Inicializado o semaforo com 1 para exclusão mútua.
State state[NUM_PHILOSOPHERS]; // Estado de cada filosofo.
semaphore s[NUM_PHILOSOPHERS]; // Semáforo para cada filosofo.
std::queue<int> fifo_queue; // Fila de prioridade.

/**
 * @brief Inicializa os semáforos e estados dos filósofos.
 */
void initialize() {
	sem_init(&mutex, 0, 1); // Inicializa o semáforo mutex com 1.
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) { // Inicializa os semáforos s com 0 e os estados dos filósofos com THINKING.
		sem_init(&s[i], 0, 0); 
		state[i] = THINKING; 
	}
}

/**
 * @brief Testa se o filósofo pode comer.
 * @param i Identificador do filósofo.
 */
void test(int i) { // Testa se o filosofo i pode comer.
	// Se o filosofo i está com fome e os vizinhos não estão comendo, então o filosofo i começa a comer.
	if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) { 
		state[i] = EATING;
		sem_post(&s[i]); // Libera o semáforo do filosofo i.
	}
}

/**
 * @brief Função que simula o ato de comer do filósofo.
 * @param i Identificador do filósofo.
 */
void eat(int i) {
	sem_wait(&mutex); // Entra na região crítica.
	std::cout << "O filoso " << i << " está comendo." << std::endl;
	sem_post(&mutex); // Sai da região crítica.
	sleep(5);
}

/**
 * @brief Função que simula o ato de pensar do filósofo.
 * @param i Identificador do filósofo.
 */
void think(int i) {
	sem_wait(&mutex); // Entra na região crítica.
	std::cout << "O filoso " << i << " está pensando." << std::endl;
	sem_post(&mutex); // Sai da região crítica.
	sleep(5);
}

/**
 * @brief Filósofo tenta pegar os garfos para comer.
 * @param i Identificador do filósofo.
 */
void takeForks(int i) {
	sem_wait(&mutex); // Entra na região crítica.
	fifo_queue.push(i); // Adiciona o filosofo i na fila de prioridade.
	std::cout << "O filoso " << i << " está com fome." << std::endl;
	state[i] = HUNGRY; // O filosofo i está com fome.

	// Enquanto o filosofo i não for o primeiro da fila ou não estiver comendo.
	while (fifo_queue.front() != i || state[i] != EATING) { 
		test(i); // Testa se o filosofo i pode comer.
		if (state[i] == EATING) break; // Se o filosofo i estiver comendo, então sai do loop.
		sem_post(&mutex); // Sai da região crítica.
		sem_wait(&s[i]); // Bloqueia o semáforo do filosofo i.
		sem_wait(&mutex); // Entra na região crítica.
	}

	fifo_queue.pop(); // Remove o filosofo i da fila de prioridade.
	sem_post(&mutex); // Sai da região crítica.
}

/**
 * @brief Filósofo devolve os garfos após comer.
 * @param i Identificador do filósofo.
 */
void putForks(int i) {
	sem_wait(&mutex); // Entra na região crítica.
	std::cout << "O filoso " << i << " terminou de comer. " << std::endl;
	state[i] = THINKING; // O filosofo i está pensando.
	test(LEFT); // Testa se o vizinho da esquerda do filosofo i pode comer.
	test(RIGHT); // Testa se o vizinho da direita do filosofo i pode comer.
	sem_post(&mutex); // Sai da região crítica.
}

/**
 * @brief Função principal da thread do filósofo.
 * @param i Ponteiro para o identificador do filósofo.
 * @return void*
 */
void* philosopher (void* i) {
	int id = *(int*)i; // Identificador do filosofo.
	while (true) { // Loop infinito.
		think(id); // O filosofo i está pensando.
		takeForks(id); // O filosofo i tenta pegar os garfos.
		eat(id); // O filosofo i está comendo.
		putForks(id); // O filosofo i devolve os garfos.
	}	
}

/**
 * @brief Função principal que cria as threads dos filósofos e inicializa o sistema.
 * @param argc Número de argumentos.
 * @param argv Argumentos.
 * @return int
 */
int main(int argc, char** argv) {
	pthread_t philosophers[NUM_PHILOSOPHERS]; // Threads dos filosofos.
	int ids[NUM_PHILOSOPHERS]; // Identificadores dos filosofos.
	initialize(); // Inicializa os semáforos e estados dos filósofos.
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) { // Cria as threads dos filosofos.
		ids[i] = i;
		pthread_create(&philosophers[i], NULL, philosopher, (void*)&ids[i]); // Cria a thread do filosofo i.
	}

	for (int i = 0; i < NUM_PHILOSOPHERS; i++) { // Espera as threads dos filosofos terminarem.
		pthread_join(philosophers[i], NULL); 
	}

	return 0;
}