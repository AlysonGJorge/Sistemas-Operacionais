#include <iostream>
#include <vector>
#include <list>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <random>
#include <map>
#include <queue>

#define NUM_PADAWANS 5
#define NUM_ESPECTADORES 10
#define NUM_TESTES 3

// Variaveis globais
int numPadawans = NUM_PADAWANS;
int numEspectadores = NUM_ESPECTADORES;
int numTestes = NUM_ESPECTADORES;

// Utilitarios

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
	pthread_cond_t condTranca;

	int capacidadeMax;
	std::map<int, bool> padawansNaSala;
	std::queue<int> padawansNaEntrada;
	int espectadoresCount;
	bool salaoAberto = false;

public:


	MonitorSalao(int capacidadeMax): capacidadeMax(capacidadeMax), padawansNaSala(), espectadoresCount(0), salaoAberto(false) {
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&condPadawan, NULL);
		pthread_cond_init(&condEspectador, NULL);
	}

	~MonitorSalao() {
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&condPadawan);
		pthread_cond_destroy(&condEspectador);
	}


	std::map<int, bool> getPadawansNaSala() {
		return this->padawansNaSala;
	}

	bool estaAprovado(int padawan) {
		return this->padawansNaSala[padawan];
	}

	void aprovarPadawan(int padawan) {
		this->padawansNaSala[padawan] = true;
	}

	void decrementaCapacidade() {
		this->capacidadeMax--;
	}

	void setCapacidadeMax(int capacidadeMax) {
		this->capacidadeMax = capacidadeMax;
	}

	int getCapacidadeMax(){
		return this->capacidadeMax;
	}


	// Padawan
	void entraSalaoPadawan(int id) {
		pthread_mutex_lock(&mutex);

		padawansNaEntrada.push(id);
		while (padawansNaEntrada.front() != id ||!salaoAberto) {
			pthread_cond_wait(&condPadawan, &mutex);
		}

		padawansNaEntrada.pop();

		this->padawansNaSala[id] = false;
		std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
		cumprimentaMestresAvaliadores(id);

		pthread_mutex_unlock(&mutex);
	}

	void saiSalaoPadawan(int id) {
		pthread_mutex_lock(&mutex);

		padawansNaSala.erase(id);
		std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
		this->decrementaCapacidade();

		pthread_mutex_unlock(&mutex);
	}

	void cumprimentaMestresAvaliadores(int id) {
		std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;
	}

	void aguardaAvaliacao(int id) {
		pthread_mutex_lock(&mutex);
		std::cout << "Padawan_" << id << " aguardando a avaliacao" << std::endl;

		while(this->salaoAberto) {
			pthread_cond_wait(&condPadawan, &mutex);
		}

		pthread_mutex_unlock(&mutex);
	}

	void realizaTeste(int id) {
		pthread_mutex_lock(&mutex);
		std::cout << "Padawan_" << id << " realizando teste" << std::endl;
		pthread_mutex_unlock(&mutex);
	}

	void aguardaCortaTranca(int id){
		pthread_mutex_lock(&mutex);

		
		pthread_cond_wait(&condTranca, &mutex);
		
		std::cout << "Padawan_" << id << " esta aguardando sua tranca ser cortada" << std::endl;
		pthread_mutex_unlock(&mutex);
	}

	void cumprimentaYoda (int id) {
		pthread_mutex_lock(&mutex);
		std::cout << "Padawan_" << id << " decepcionadamente cumprimenta Yoda" << std::endl;
		pthread_mutex_unlock(&mutex);
	}


	// Espectador

	void entraSalaoEspectador(int id) {
		pthread_mutex_lock(&mutex);

		while (!salaoAberto) {
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
	
		pthread_mutex_unlock(&mutex);		
	}

	void assisteTestes(int id) {
		pthread_mutex_lock(&mutex);
		std::cout << "Espectador_" << id << " assistindo os testes" << std::endl;
		pthread_mutex_unlock(&mutex);
	}
	
	// Yoda

	void iniciarTestes() {
		pthread_mutex_lock(&mutex);
		std::cout << "[Yoda]: Testes iniciados." << std::endl;
		salaoAberto = false;
		pthread_mutex_unlock(&mutex);
	}

	void finalizarTestes() {
		pthread_mutex_lock(&mutex);
		salaoAberto = true;
		std::cout << "[Yoda]: Testes finalizados." << std::endl;
		discursoYoda();
		pthread_mutex_unlock(&mutex);
	}

	void anunciaResultado(std::map<int, bool> padawans) {
		pthread_mutex_lock(&mutex);
		std::random_device rd;
		std::mt19937 g(rd());

		std::cout << "[Yoda]: O teste concluído foi. Os resultados anunciarei!" << std::endl;

		std::cout << "[Yoda]: Os aprovados estes são: ";

		std::list<int> aprovados;
		for (int i = 0; i < padawans.size() ; i++) {
			int pass = rand() % 100 + 1;
			if (pass >= 50){
				std::cout << "Padawan_" << i << " ";
				aprovados.push_back(i);
				this->aprovarPadawan(i);
			}
		}

		cortaTranca(aprovados);

		std::cout << "[Yoda]: Todos os aprovados jedi se tornaram. Por encerrada a cerimônia, declaro!" << std::endl;

		
		pthread_cond_signal(&condTranca);
		pthread_mutex_unlock(&mutex);
		

	};

	void cortaTranca(std::list<int> &padawansAprovados) {
		std::cout << "[Yoda]: A cerimônia de cortes de trança começa agora..." << std::endl;
		
		for (auto padawan : padawansAprovados) {
			std::cout << "[Yoda]: Padawan_" << padawan 
			<< ", sua trança cortada será. Jedi agora você é" << std::endl;

			std::cout << "Padawan_" << padawan << " teve sua trança cortada" << std::endl;
		}

	}

	void liberarEntrada() {
		pthread_mutex_lock(&mutex);

		int time = rand() % 10 + 1;
		std::cout << "[Yoda]: Por " << time << " segundos, a entrada liberarei" << std::endl;
		std::cout << "Yoda liberou a entrada para os próximos testes" << std::endl;

		salaoAberto = true;

		while(time > 0 && !padawansNaEntrada.empty()){
			pthread_cond_signal(&condPadawan);
			sleep(1);
			time--;
		}	
		

		pthread_mutex_unlock(&mutex);
	}

};


MonitorSalao monitorSalao(NUM_PADAWANS); // Monitor global

// Threads

void* yodaThread(void* arg) {
	std::cout << "Yoda foi criado" << std::endl;
	while (monitorSalao.getCapacidadeMax() != 0){
		monitorSalao.liberarEntrada();
		sleep(1);
		monitorSalao.iniciarTestes();
		sleep(1);
		monitorSalao.anunciaResultado(monitorSalao.getPadawansNaSala());
		sleep(1);
	}

	monitorSalao.finalizarTestes();

	pthread_exit(EXIT_SUCCESS);
}

void* padawanThread(void* arg) {
	int id = *(int*)arg;
	delete (int*)arg;

	std::cout << "Padawan_" << id << " foi criado" << std::endl;
	monitorSalao.entraSalaoPadawan(id);
	sleep(1);
	monitorSalao.aguardaAvaliacao(id);
	sleep(1);
	monitorSalao.realizaTeste(id);
	sleep(1);

	if(monitorSalao.estaAprovado(id)){
		monitorSalao.aguardaCortaTranca(id);
		sleep(1);
		monitorSalao.saiSalaoPadawan(id);
		pthread_exit(NULL);
	}

	monitorSalao.cumprimentaYoda(id);
	sleep(1);
	monitorSalao.saiSalaoPadawan(id);
	pthread_exit(NULL);

}


void* espectadorThread(void* arg) {
	int id = *(int*)arg;
	delete (int*)arg;

	std::cout << "Espectador_" << id << " foi criado" << std::endl;
	monitorSalao.entraSalaoEspectador(id);
	sleep(1);
	monitorSalao.assisteTestes(id);	
	sleep(1);
	monitorSalao.saiSalaoEspectador(id);
	pthread_exit(NULL);
}


int main(int argc, char** argv) {
	srand(time(NULL));

	std::cout << "Digite o numero de padawans: ";
	std::cin >> numPadawans;
	std::cout << "Digite o numero de espectadores: ";
	std::cin >> numEspectadores;
	
	pthread_t padawans[numPadawans];
	pthread_t espectadores[numEspectadores];
	pthread_t yoda;

	monitorSalao.setCapacidadeMax(numPadawans);

	pthread_create(&yoda, NULL, yodaThread, NULL);


	for (int i = 0; i < numPadawans; i++) {
		int *id = new int(i);
		pthread_create(&padawans[i], NULL, padawanThread, (void*)id);
	}

	for (int i = 0; i < numEspectadores; i++) {
		int *id = new int(i);
		pthread_create(&espectadores[i], NULL, espectadorThread, (void*)id);
	}

	for (int i = 0; i < numPadawans; i++) {
    	pthread_join(padawans[i], NULL);
	}

	for (int i = 0; i < numEspectadores; i++) {
    	pthread_join(espectadores[i], NULL);
	}

	pthread_join(yoda, NULL);


	return 0;
}