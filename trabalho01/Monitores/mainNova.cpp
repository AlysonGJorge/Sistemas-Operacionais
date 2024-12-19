#include <iostream>
#include <unistd.h>
#include <pthread.h>

#define NUM_PADAWANS 5
#define NUM_ESPECTADORES 10

int numPadawans = NUM_PADAWANS;
int numEspectadores = NUM_ESPECTADORES;

// Utilitario
void discursoYoda() {
	std::cout << "Yoda começou a discursar" << std::endl;
	std::cout << "[Yoda]:A avaliação, concluída está. Muitos desafios enfrentaram, mas a força dentro de cada um de vocês, clara foi." << std::endl;

	std::cout << " [Yoda]: Alguns de vocês, Jedi se tornam, outros, o caminho ainda terão que percorrer. \nO sucesso, mais que habilidade demonstra, a coragem, a perseverança e a sabedoria que cada um possui em seu coração." << std::endl;

	std::cout << "[Yoda]: Lembre-se, o aprendizado nunca termina. A jornada, longa é. \nEm vocês, jovens Padawans, vejo o futuro da Ordem. Não importa o resultado, todos cresceram e aprenderam algo valioso." << std::endl;

	std::cout << "[Yoda]: A Força, sempre os guiará. Mantenham-se firmes em seu propósito, e nunca duvidem de si mesmos, pois grandes feitos estão diante de vocês. Que a Força os acompanhe, hoje e sempre." << std::endl;
}



class MonitorSalao {
	private:
		pthread_mutex_t mutex;
		pthread_cond_t condPadawan;
		pthread_cond_t condEspectador;
		pthread_cond_t condYoda;

		int totalPadawans;
		int countEmTeste;
		bool salaoAberto;
		bool todosEmSala;

	public:
		MonitorSalao(int totalPadawans): totalPadawans(totalPadawans), countEmTeste(0), 
		salaoAberto(false), todosEmSala(false) {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&condPadawan, NULL);
			pthread_cond_init(&condEspectador, NULL);
			pthread_cond_init(&condYoda, NULL);
		}

		~MonitorSalao() {
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&condPadawan);
			pthread_cond_destroy(&condEspectador);
			pthread_cond_destroy(&condYoda);
		}

		void setTotalPadawans(int totalPadawans) {
			this->totalPadawans = totalPadawans;
		}

		// Yoda

		void liberarEntrada() {
			pthread_mutex_lock(&mutex);
			int time = rand() % 10 + 1;

			std::cout << "[Yoda]: Por " << time << " segundos, a entrada liberarei" << std::endl;
			std::cout << "Yoda liberou a entrada para os próximos testes" << std::endl;
			salaoAberto = true;
			pthread_cond_broadcast(&condPadawan);
			pthread_cond_broadcast(&condEspectador);

			pthread_mutex_unlock(&mutex);
			
			sleep(time);

			pthread_mutex_lock(&mutex);
			salaoAberto = false;
			pthread_mutex_unlock(&mutex);
		}

		void iniciarTestes() {
			pthread_mutex_lock(&mutex);
			std::cout << "Yoda iniciou os testes" << std::endl;
			countEmTeste = totalPadawans;
			pthread_mutex_unlock(&mutex);
		}


		// Padawan

		void entraSalaoPadawan(int id) {
			pthread_mutex_lock(&mutex);
			while (!this->salaoAberto) {
				pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
			}
			std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
			pthread_mutex_unlock(&mutex);

		}

};

// Monitor Global
MonitorSalao monitorSalao(NUM_PADAWANS);


// Threads

void *yodaThread(void *arg) {
	std::cout << "Yoda foi criado" << std::endl;
	discursoYoda();
	pthread_exit(NULL);
}

void *padawanThread(void *arg) {
	int id = *(int *)arg;
	
	pthread_exit(NULL);
}

void *espectadorThread(void *arg) {
	int id = *(int *)arg;
	
	pthread_exit(NULL);
}

int main (int argc, char **argv) {
	pthread_t yoda;

	std::cout << "Digite o numero de padawans: ";
	std::cin >> numPadawans;
	std::cout << "Digite o numero de espectadores: ";
	std::cin >> numEspectadores;

	monitorSalao.setTotalPadawans(numPadawans);
	pthread_t padawans[numPadawans];
	pthread_t espectadores[numEspectadores];

	for(int i = 0; i < numPadawans; i++) {
		int id = i;
		std::cout << "Padawan_" << id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id);
	}

	for(int i = 0; i < numEspectadores; i++) {
		int id = i;
		std::cout << "Espectador_" << id << " foi criado" << std::endl;
		pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id);
	}

	pthread_create(&yoda, NULL, yodaThread, NULL);

	for(int i = 0; i < numPadawans; i++) {
		pthread_join(padawans[i], NULL);
	}

	for(int i = 0; i < numEspectadores; i++) {
		pthread_join(espectadores[i], NULL);
	}

	pthread_join(yoda, NULL);

	return 0;
}