#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <list>

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
		pthread_cond_t condCortaTranca;

		int totalPadawans;
		std::list<int> padawansNoSalao;
		std::list<int> padawansAprovados;
		bool primeiraExecucao;
		int countEmTeste;
		bool salaoAberto;
		bool testeEmAndamento;
		bool qntdTesteSetada;

	public:
		MonitorSalao(int totalPadawans): totalPadawans(totalPadawans), countEmTeste(0), 
		salaoAberto(false), primeiraExecucao(true), testeEmAndamento(false), 
		qntdTesteSetada(false), padawansNoSalao(), padawansAprovados() {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&condPadawan, NULL);
			pthread_cond_init(&condEspectador, NULL);
			pthread_cond_init(&condYoda, NULL);
			pthread_cond_init(&condCortaTranca, NULL);
		}

		~MonitorSalao() {
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&condPadawan);
			pthread_cond_destroy(&condEspectador);
			pthread_cond_destroy(&condYoda);
			pthread_cond_destroy(&condCortaTranca);
		}

		void setTotalPadawans(int totalPadawans) {
			this->totalPadawans = totalPadawans;
		}

		void decrementaTotalPadawans() {
			this->totalPadawans--;
		}

		bool estaAprovado(int id) {
			pthread_mutex_lock(&mutex);
			for (auto& padawan : this->padawansAprovados){
				if (padawan == id) {
					return true;
				}
			}

			return false;
			pthread_mutex_unlock(&mutex);
		}

		// Yoda

		void liberarEntrada() {
			pthread_mutex_lock(&mutex);
			int time = rand() % 10 + 1;

			while (!primeiraExecucao && this->padawansNoSalao.size() > 0) {
				pthread_cond_wait(&condYoda, &mutex);
			}

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

			while (countEmTeste > 0) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			std::cout << "[Yoda]: Em teste, todos estão. O teste começará em breve." << std::endl;
			testeEmAndamento = true;
			qntdTesteSetada = false;

			pthread_cond_broadcast(&condPadawan);

			pthread_mutex_unlock(&mutex);
		}

		void anunciaResultado() {
			pthread_mutex_lock(&mutex);

			while(this->countEmTeste > 0) {
				pthread_cond_wait(&condYoda, &mutex);
			}
			
			std::cout << "[Yoda]: Concluído o teste foi. Os Resultados anunciados em breve serão." << std::endl;
			
			for (auto& padawan : padawansNoSalao){
				int r = rand() % 100 +1;
				if(r >= 50) {
					std::cout << "[Yoda]: Padawan_" << padawan << " foi aprovado" << std::endl;
					this->padawansAprovados.push_back(padawan);
				}
			}

			this->testeEmAndamento = false;
			this->qntdTesteSetada = false;
			pthread_cond_broadcast(&condPadawan);
			

			pthread_mutex_unlock(&mutex);
		}

		void cortaTranca(){
			pthread_mutex_lock(&mutex);
			std::cout << "[Yoda]: A cerimônia de corte de trança começará em breve" << std::endl;

			while(countEmTeste < this->padawansAprovados.size()) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			for(auto& padawan : padawansAprovados) {
				std::cout << "[Yoda]: Padawan_" << padawan << " sua trança cortada teve. " << std::endl;
				this->padawansAprovados.remove(padawan);
			}
			
			pthread_cond_broadcast(&condCortaTranca);
			pthread_mutex_unlock(&mutex);
		}

		void finalizaTestes() {
			pthread_mutex_lock(&mutex);
			std::cout << "[Yoda]: Por fim, os testes todos se encerram." << std::endl;
			discursoYoda();
			pthread_mutex_unlock(&mutex);
		}


		// Padawan

		void entraSalaoPadawan(int id) {
			pthread_mutex_lock(&mutex);
			while (!this->salaoAberto) {
				pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
			}
			std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
			cumprimentaMestresAvaliadores(id);
			this->padawansNoSalao.push_back(id);
			pthread_mutex_unlock(&mutex);
		}

		void saiSalaoPadawan(int id) {
			pthread_mutex_lock(&mutex);

			while (this->testeEmAndamento){
				pthread_cond_wait(&condPadawan, &mutex);
			}

			std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
			this->padawansNoSalao.remove(id);

			if (this->padawansNoSalao.size() == 0 && !this->primeiraExecucao) {
				pthread_cond_signal(&condYoda);
			}

			pthread_mutex_unlock(&mutex);
		}

		void aguardaTestePadawan(int id) {
			pthread_mutex_lock(&mutex);
			
			if(!qntdTesteSetada) {
				countEmTeste = totalPadawans;
				qntdTesteSetada = true;
			}

			countEmTeste--;
			
			if (this->countEmTeste == 0) {
				pthread_cond_signal(&condYoda);
			}

			while(!this->testeEmAndamento) {
				std::cout << "Padawan_" << id << " aguardando teste" << std::endl;
				pthread_cond_wait(&condPadawan, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}

		void realizaTeste(int id) {
			pthread_mutex_lock(&mutex);
			
			if (!this->qntdTesteSetada) {
				countEmTeste = this->padawansNoSalao.size();
				qntdTesteSetada = true;
			}

			countEmTeste--;

			std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;

			if(this->countEmTeste == 0) {
				pthread_cond_signal(&condYoda);
			}
			
			while (this->testeEmAndamento) {
				pthread_cond_wait(&condPadawan, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}

		void aguardaCortaTranca(int id) {
			pthread_mutex_lock(&mutex);
			countEmTeste++;

			if(countEmTeste == this->padawansAprovados.size()) {
				pthread_cond_signal(&condYoda);
			}

			std::cout << "Padawan_" << id << " aguardando corte de tranca" << std::endl;
			while (this->padawansAprovados.size() > 0) {
				pthread_cond_wait(&condCortaTranca, &mutex);
			}
			pthread_mutex_unlock(&mutex);
		}

		void cumprimentaMestresAvaliadores(int id) {
			std::cout << "Padawan_" << id << "cumprimentou os mestres avaliadores" << std::endl;

		}
		
		void cumprimentaYoda(int id) {
			std::cout << "Padawan_" << id << "decepcionadamente cumprimenta Yoda" << std::endl;
		}

};

// Monitor Global
MonitorSalao monitorSalao(NUM_PADAWANS);


// Threads

void *yodaThread(void *arg) {
	std::cout << "Yoda foi criado" << std::endl;
	monitorSalao.liberarEntrada();
	monitorSalao.iniciarTestes();
	monitorSalao.anunciaResultado();
	monitorSalao.cortaTranca();
	monitorSalao.finalizaTestes();

	pthread_exit(NULL);
}

void *padawanThread(void *arg) {
	int id = *(int *)arg;
	monitorSalao.entraSalaoPadawan(id);
	monitorSalao.aguardaTestePadawan(id);
	monitorSalao.realizaTeste(id);

	if (monitorSalao.estaAprovado(id)) {
		monitorSalao.aguardaCortaTranca(id);
		
	}

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
	// std::cout << "Digite o numero de espectadores: ";
	// std::cin >> numEspectadores;

	monitorSalao.setTotalPadawans(numPadawans);
	pthread_t padawans[numPadawans];
	// pthread_t espectadores[numEspectadores];

	for(int i = 0; i < numPadawans; i++) {
		int id = i;
		std::cout << "Padawan_" << id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	int id = i;
	// 	std::cout << "Espectador_" << id << " foi criado" << std::endl;
	// 	pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id);
	// }

	pthread_create(&yoda, NULL, yodaThread, NULL);

	for(int i = 0; i < numPadawans; i++) {
		pthread_join(padawans[i], NULL);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	pthread_join(espectadores[i], NULL);
	// }

	pthread_join(yoda, NULL);

	return 0;
}