#include <iostream>
#include "models.cpp"
#include <vector>
#include <list>
#include <pthread.h>
#include <ctime>
#include <cstdlib>

#define NUM_PADAWANS 5
#define NUM_ESPECTADORES 10
#define NUM_TESTES 3

// Variaveis globais
int numPadawans = NUM_PADAWANS;
int numEspectadores = NUM_ESPECTADORES;
int numTestes = NUM_ESPECTADORES;

class MonitorSalao {
private:
	pthread_mutex_t mutex;
	pthread_cond_t condPadawan;
	pthread_cond_t condEspectador;

	int capacidadeMax;
	// int padawansCount;
	std::list<pthread_t> padawansNaSala;
	int espectadoresCount;
	bool testeEmAndamento;

public:
	MonitorSalao(int capacidadeMax): capacidadeMax(capacidadeMax), padawansNaSala(0), espectadoresCount(0), testeEmAndamento(false) {
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&condPadawan, NULL);
		pthread_cond_init(&condEspectador, NULL);
	}

	~MonitorSalao() {
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&condPadawan);
		pthread_cond_destroy(&condEspectador);
	}

	// Padawan


	void entraSalaoPadawan(int id) {
		pthread_mutex_lock(&mutex);

		while (padawansNaSala.size() == capacidadeMax || testeEmAndamento) {
			pthread_cond_wait(&condPadawan, &mutex);
		}

		padawansNaSala.push_back(pthread_self());
		std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
		pthread_mutex_unlock(&mutex);
	}

	void saiSalaoPadawan(int id) {
		pthread_mutex_lock(&mutex);

		padawansNaSala.remove(pthread_self());
		std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
		pthread_cond_signal(&condPadawan);

		pthread_mutex_unlock(&mutex);
	}

	// Espectador

	void entraSalaoEspectador(int id) {
		pthread_mutex_lock(&mutex);

		while((espectadoresCount + padawansNaSala.size()) == capacidadeMax || testeEmAndamento) {
			pthread_cond_wait(&condEspectador, &mutex);
		}

		espectadoresCount++;
		std::cout << "Espectador_" << id << " entrou no salao" << std::endl;
		pthread_mutex_unlock(&mutex);
	}

	void saiSalaoEspectador(int id) {
		pthread_mutex_lock(&mutex);
		espectadoresCount--;
		std::cout << "Espectador_" << id << " saiu do salao" << std::endl;
		pthread_cond_signal(&condEspectador);
		pthread_mutex_unlock(&mutex);		
	}

	// Yoda

	void iniciarTestes() {
		pthread_mutex_lock(&mutex);
		testeEmAndamento = true;
		pthread_mutex_unlock(&mutex);
	}

	void finalizarTestes() {
		pthread_mutex_lock(&mutex);
		testeEmAndamento = false;
		std::cout << "[Yoda]: Testes finalizados." << std::endl;
		discursoYoda();
		exit(EXIT_SUCCESS);
		pthread_mutex_unlock(&mutex);
	}

	void anunciaResultado(unsigned int time, std::vector<pthread_t> &padawans) {
		pthread_mutex_lock(&mutex);
		testeEmAndamento = false;
		std::cout << "[Yoda]: Teste finalizados" << std::endl;

		int numPadawansAprovados = rand() % padawans.size();
		std::list<pthread_t> padawansAprovados;

		for (int i = 0; i < numPadawansAprovados; i++) {
			padawansAprovados.push_back(padawans[rand() % padawans.size()]);
		}

		std::cout << "[Yoda]: " << numPadawansAprovados << " Padawans aprovados" << std::endl;
		std::cout << "[Yoda]: Foram aprovados: ";
		for (auto padawan : padawansAprovados) {
			std::cout << padawan << " ";
		}

		std::cout << "[Yoda]: Irei esperar " << time << " segundos" 
			<< "antes de liberar a entrada" << std::endl; 
		sleep(time);
		pthread_mutex_unlock(&mutex);
		liberarEntrada();
	};


	void liberarEntrada() {
		pthread_mutex_lock(&mutex);
		int numPadawansParaEntrar = rand() % capacidadeMax;
		for (int i = 0; i < numPadawansParaEntrar; i++) {
			pthread_cond_signal(&condPadawan);
		}
		for (int i = 0; i < (capacidadeMax - numPadawansParaEntrar); i++) {
			pthread_cond_signal(&condEspectador);
		}
		pthread_mutex_unlock(&mutex);
	}

};

// Utilitarios

void discursoYoda() {
	std::cout << "[Yoda]:A avaliação, concluída está. Muitos desafios enfrentaram, mas a força dentro de cada um de vocês, clara foi." << std::endl;

	std::cout << " [Yoda]: Alguns de vocês, Jedi se tornam, outros, o caminho ainda terão que percorrer. \nO sucesso, mais que habilidade demonstra, a coragem, a perseverança e a sabedoria que cada um possui em seu coração." << std::endl;

	std::cout << "[Yoda]: Lembre-se, o aprendizado nunca termina. A jornada, longa é. \nEm vocês, jovens Padawans, vejo o futuro da Ordem. Não importa o resultado, todos cresceram e aprenderam algo valioso." << std::endl;

	std::cout << "[Yoda]: A Força, sempre os guiará. Mantenham-se firmes em seu propósito, e nunca duvidem de si mesmos, pois grandes feitos estão diante de vocês. Que a Força os acompanhe, hoje e sempre." << std::endl;
}

int main(int argc, char** argv) {
	srand(time(NULL));

	std::cout << "Digite o numero de padawans: ";
	std::cin >> numPadawans;
	std::cout << "Digite o numero de espectadores: ";
	std::cin >> numEspectadores;
	std::cout << "Digite o numero de testes: ";
	std::cin >> numTestes;

	return 0;
}