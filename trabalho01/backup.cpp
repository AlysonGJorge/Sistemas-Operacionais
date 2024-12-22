#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <algorithm>

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
		pthread_cond_t condPadawanSalao;
		pthread_cond_t condTeste;
		pthread_cond_t condEspectador;
		pthread_cond_t condYoda;
		pthread_cond_t condCortaTranca;

		int totalPadawans;
		std::list<int> padawansNoSalao;
		std::list<int> padawansAprovados;
		std::queue<int> filaPadawans;
		bool primeiraExecucao;
		int countEsperandoTeste;
		int countEmTeste;
		bool salaoAberto;
		bool padawanEmTeste;
		bool todosPadawansEmTeste;
		bool qntdTesteSetada;

	public:
		MonitorSalao(int totalPadawans): totalPadawans(totalPadawans), primeiraExecucao(true), countEsperandoTeste(0), countEmTeste(0), salaoAberto(false), padawanEmTeste(false), todosPadawansEmTeste(false), qntdTesteSetada(false) {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&condPadawan, NULL);
			pthread_cond_init(&condPadawanSalao, NULL);
			pthread_cond_init(&condTeste, NULL);
			pthread_cond_init(&condEspectador, NULL);
			pthread_cond_init(&condYoda, NULL);
			pthread_cond_init(&condCortaTranca, NULL);
		}

		~MonitorSalao() {
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&condPadawan);
			pthread_cond_destroy(&condPadawanSalao);
			pthread_cond_destroy(&condTeste);
			pthread_cond_destroy(&condEspectador);
			pthread_cond_destroy(&condYoda);
			pthread_cond_destroy(&condCortaTranca);
		}

		void setTotalPadawans(int totalPadawans) {
			this->totalPadawans = totalPadawans;
		}

		int getTotalPadawans() {
			return this->totalPadawans;
		}

		void decrementaTotalPadawans() {
			this->totalPadawans--;
		}

		bool estaAprovado(int id) {
			pthread_mutex_lock(&mutex);
			bool aprovado = std::find(padawansAprovados.begin(), padawansAprovados.end(), id) != padawansAprovados.end();
			pthread_mutex_unlock(&mutex);
			return aprovado;
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
			pthread_cond_signal(&condPadawan);
			pthread_cond_signal(&condEspectador);

			pthread_mutex_unlock(&mutex);
			sleep(time);

			pthread_mutex_lock(&mutex);
			salaoAberto = false;
			primeiraExecucao = false;
			pthread_cond_signal(&condPadawanSalao);
			
			while (countEsperandoTeste > 0) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}

		void iniciarTestes() {
			pthread_mutex_lock(&mutex);

			std::cout << "[Yoda]: Em teste, todos estão. O teste começará em breve." << std::endl;
			todosPadawansEmTeste = true;
			qntdTesteSetada = false;

			pthread_cond_signal(&condPadawanSalao);
			


			pthread_mutex_unlock(&mutex);
		}

		void anunciaResultado() {
			pthread_mutex_lock(&mutex);

			
			bool fuc = false;
			while(!filaPadawans.empty()) {
				if(!fuc) {
					// std::cout << std::endl << "Estou tentando acordar os padawans que estão na fila" << std::endl;
					fuc = true;
					std::cout << "Padawan_" << this->filaPadawans.front() << " acordado" << std::endl;
					std::cout << std::endl << "Yoda esta preso " << std::endl;
					std::cout << std::endl << "Tamanho da fila " << padawansNoSalao.size() << std::endl;
				}
				pthread_cond_signal(&condTeste);
			}

			std::cout << std::endl << "Primeiro da fila: " << filaPadawans.front() << std::endl;

			while(countEmTeste > 0) {
				// std::cout << std::endl << "Estou esperando todos os padawans estarem realizando os teste " << std::endl;
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

			this->todosPadawansEmTeste = false;
			this->qntdTesteSetada = false;
			pthread_cond_signal(&condPadawanSalao);
			

			pthread_mutex_unlock(&mutex);
		}

		void cortaTranca(){
			pthread_mutex_lock(&mutex);
			std::cout << "[Yoda]: A cerimônia de corte de trança começará em breve" << std::endl;

			while(countEsperandoTeste < (int) this->padawansAprovados.size()) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			for(auto& padawan : padawansAprovados) {
				std::cout << "[Yoda]: Padawan_" << padawan << " sua trança cortada teve. " << std::endl;
			}

			this->padawansAprovados.clear();
			
			pthread_cond_signal(&condCortaTranca);
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
			while (!salaoAberto) {
				pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
			}

			pthread_cond_signal(&condPadawan);
			this->padawansNoSalao.push_back(id);
			this->filaPadawans.push(id);
			std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
			cumprimentaMestresAvaliadores(id);

			pthread_mutex_unlock(&mutex);
		}

		void saiSalaoPadawan(int id, bool aprovado = false) {
			pthread_mutex_lock(&mutex);

			while (this->todosPadawansEmTeste){
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}

			if (aprovado) {
				pthread_cond_signal(&condCortaTranca);
			}

			std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
			this->padawansNoSalao.remove(id);

			if (this->padawansNoSalao.size() == 0 && !this->primeiraExecucao) {
				pthread_cond_signal(&condYoda);
			}

			this->decrementaTotalPadawans();

			pthread_mutex_unlock(&mutex);
		}

		void aguardaTestePadawan(int id) {
			pthread_mutex_lock(&mutex);
			
			pthread_cond_signal(&condPadawanSalao);
			std::cout << "Padawan_" << id << " está aguardando teste" << std::endl;


			while(salaoAberto){
				
				// std::cout << std::endl << "To preso =(" << std::endl;
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}



			if(!qntdTesteSetada) {
				// std::cout << std::endl << "ENTRARAM " << this->padawansNoSalao.size() << " padawans no salao" << std::endl;
				countEmTeste = this->padawansNoSalao.size();
				qntdTesteSetada = true;
			}


			// std::cout << std::endl << "COUNT: " << countEsperandoTeste << std::endl;
			
			countEmTeste--;

			pthread_cond_signal(&condPadawanSalao);
			if (this->countEmTeste == 0) {
				pthread_cond_signal(&condYoda);
			}
			
			// std::cout << std::endl << "Acho que escapei" << std::endl;

			pthread_mutex_unlock(&mutex);
		}

		void realizaTeste(int id) {
			pthread_mutex_lock(&mutex);

			// std::cout << std::endl << "Tamanho da fila: " << this->filaPadawans.size() << std::endl;
			std::cout << std::endl << "Primeiro da fila: " << filaPadawans.front() << std::endl;
			
			while(filaPadawans.front() != id){
				std::cout << "Padawan_" << id << " está esperando o teste na fila" << std::endl;
				pthread_cond_wait(&condTeste, &mutex);
				
			}
			
			pthread_cond_signal(&condTeste);
			filaPadawans.pop();

			std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;
			// pthread_cond_signal(&condTeste);
			
			// pthread_cond_signal(&condYoda);

			// pthread_cond_signal(&condPadawanSalao);

			if (!this->qntdTesteSetada) {
				// std::cout << std::endl << "ENTRARAM " << this->padawansNoSalao.size() << " padawans no salao" << std::endl;
				countEmTeste = this->padawansNoSalao.size();
				qntdTesteSetada = true;
			}

			countEmTeste--;


			if(this->countEmTeste == 0) {
				// padawanEmTeste = true;
				pthread_cond_signal(&condYoda);
			}

			while (this->todosPadawansEmTeste) {
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}


		void aguardaCortaTranca(int id) {
			pthread_mutex_lock(&mutex);
			countEsperandoTeste++;

			

			pthread_cond_signal(&condPadawanSalao);

			if(countEsperandoTeste == (int) this->padawansAprovados.size()) {
				pthread_cond_signal(&condYoda);
			}

			std::cout << "Padawan_" << id << " aguardando corte de tranca" << std::endl;
			while (this->padawansAprovados.size() > 0) {
				pthread_cond_wait(&condCortaTranca, &mutex);
			}
			pthread_mutex_unlock(&mutex);
		}

		void cumprimentaMestresAvaliadores(int id) {
			std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;

		}
		
		void cumprimentaYoda(int id) {
			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&condPadawanSalao);
			std::cout << "Padawan_" << id << " decepcionadamente cumprimenta Yoda" << std::endl;
			pthread_mutex_unlock(&mutex);
		}

};

// Monitor Global
MonitorSalao monitorSalao(NUM_PADAWANS);


// Threads

void *yodaThread(void *arg) {
	std::cout << "Yoda foi criado" << std::endl;
	while (monitorSalao.getTotalPadawans() > 0) {
		monitorSalao.liberarEntrada();
		monitorSalao.iniciarTestes();
		monitorSalao.anunciaResultado();
		monitorSalao.cortaTranca();

	}
	monitorSalao.finalizaTestes();

	pthread_exit(NULL);
}

void *padawanThread(void *arg) {
	int id = *(int *)arg;
	delete (int *)arg;

	monitorSalao.entraSalaoPadawan(id);
	monitorSalao.aguardaTestePadawan(id);
	monitorSalao.realizaTeste(id);

	if (monitorSalao.estaAprovado(id)) {
		monitorSalao.aguardaCortaTranca(id);
		monitorSalao.saiSalaoPadawan(id, true);

		std::cout << "Padawan_" << id << " foi aprovado e vai sair do salao" << std::endl;
		pthread_exit(NULL);
	}

	monitorSalao.cumprimentaYoda(id);
	monitorSalao.saiSalaoPadawan(id);

	std::cout << "Padawan_" << id << " foi reprovado e vai sair do salao" << std::endl;
	pthread_exit(NULL);
}

void *espectadorThread(void *arg) {
	int id = *(int *)arg;
	delete (int *)arg;
	
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
		int* id = new int(i);
		std::cout << "Padawan_" << *id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	int id = i;
	// 	std::cout << "Espectador_" << id << " foi criado" << std::endl;
	// 	pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id);
	// }

	pthread_create(&yoda, NULL, yodaThread, NULL);

	
	for (int i = 0; i < numPadawans; i++) {
    	pthread_join(padawans[i], NULL);
	}

	

	// for(int i = 0; i < numEspectadores; i++) {
	// 	pthread_join(espectadores[i], NULL);
	// }

	pthread_join(yoda, NULL);

	return 0;
}

// Ver 2

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <algorithm>

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
		pthread_cond_t condPadawanSalao;
		pthread_cond_t condTeste;
		pthread_cond_t condEspectador;
		pthread_cond_t condYoda;
		pthread_cond_t condCortaTranca;

		int totalPadawans;
		std::list<int> padawansNoSalao;
		std::list<int> padawansAprovados;
		std::queue<int> filaPadawans;
		bool primeiraExecucao;
		int countEsperandoTeste;
		int countEmTeste;
		bool salaoAberto;
		bool testesConcluidos;
		bool testeEmAndamento;
		bool qntdTesteSetada;

	public:
		MonitorSalao(int totalPadawans): totalPadawans(totalPadawans), primeiraExecucao(true), countEsperandoTeste(0), countEmTeste(0), salaoAberto(false), testesConcluidos(false), testeEmAndamento(false), qntdTesteSetada(false) {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&condPadawan, NULL);
			pthread_cond_init(&condPadawanSalao, NULL);
			pthread_cond_init(&condTeste, NULL);
			pthread_cond_init(&condEspectador, NULL);
			pthread_cond_init(&condYoda, NULL);
			pthread_cond_init(&condCortaTranca, NULL);
		}

		~MonitorSalao() {
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&condPadawan);
			pthread_cond_destroy(&condPadawanSalao);
			pthread_cond_destroy(&condTeste);
			pthread_cond_destroy(&condEspectador);
			pthread_cond_destroy(&condYoda);
			pthread_cond_destroy(&condCortaTranca);
		}

		void setTotalPadawans(int totalPadawans) {
			this->totalPadawans = totalPadawans;
		}

		int getTotalPadawans() {
			return this->totalPadawans;
		}

		void decrementaTotalPadawans() {
			this->totalPadawans--;
		}

		bool estaAprovado(int id) {
			pthread_mutex_lock(&mutex);
			bool aprovado = std::find(padawansAprovados.begin(), padawansAprovados.end(), id) != padawansAprovados.end();
			pthread_mutex_unlock(&mutex);
			return aprovado;
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
			pthread_cond_signal(&condPadawan);
			pthread_cond_signal(&condEspectador);

			pthread_mutex_unlock(&mutex);
			sleep(time);

			pthread_mutex_lock(&mutex);
			salaoAberto = false;
			primeiraExecucao = false;

			pthread_cond_signal(&condPadawanSalao);
			
			while (countEsperandoTeste > 0) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}

		void iniciarTestes() {
			pthread_mutex_lock(&mutex);

			std::cout << "[Yoda]: Em teste, todos estão. O teste começará em breve." << std::endl;
			testeEmAndamento = true;
			qntdTesteSetada = false;
			
			pthread_cond_broadcast(&condPadawanSalao);
			


			pthread_mutex_unlock(&mutex);
		}

		void anunciaResultado() {
			pthread_mutex_lock(&mutex);

			
			bool fuc = false;
			while(!filaPadawans.empty()) {
				if(!fuc) {
					// std::cout << std::endl << "Estou tentando acordar os padawans que estão na fila" << std::endl;
					fuc = true;
					std::cout << "Padawan_" << this->filaPadawans.front() << " acordado" << std::endl;
					std::cout << std::endl << "Yoda esta preso " << std::endl;
					std::cout << std::endl << "Tamanho da fila " << padawansNoSalao.size() << std::endl;
				}
				pthread_cond_signal(&condTeste);
			}

			std::cout << std::endl << "Primeiro da fila: " << filaPadawans.front() << std::endl;

			while(countEmTeste > 0) {
				// std::cout << std::endl << "Estou esperando todos os padawans estarem realizando os teste " << std::endl;
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
			pthread_cond_signal(&condPadawanSalao);
			

			pthread_mutex_unlock(&mutex);
		}

		void cortaTranca(){
			pthread_mutex_lock(&mutex);
			std::cout << "[Yoda]: A cerimônia de corte de trança começará em breve" << std::endl;

			while(countEsperandoTeste < (int) this->padawansAprovados.size()) {
				pthread_cond_wait(&condYoda, &mutex);
			}

			for(auto& padawan : padawansAprovados) {
				std::cout << "[Yoda]: Padawan_" << padawan << " sua trança cortada teve. " << std::endl;
			}

			this->padawansAprovados.clear();
			
			pthread_cond_signal(&condCortaTranca);
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
			while (!salaoAberto) {
				pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
			}

			pthread_cond_signal(&condPadawan);
			this->padawansNoSalao.push_back(id);
			this->filaPadawans.push(id);
			std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
			cumprimentaMestresAvaliadores(id);

			pthread_mutex_unlock(&mutex);
		}

		void saiSalaoPadawan(int id, bool aprovado = false) {
			pthread_mutex_lock(&mutex);

			while (this->testeEmAndamento){
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}

			if (aprovado) {
				pthread_cond_signal(&condCortaTranca);
			}

			std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
			this->padawansNoSalao.remove(id);

			if (this->padawansNoSalao.size() == 0 && !this->primeiraExecucao) {
				pthread_cond_signal(&condYoda);
			}

			this->decrementaTotalPadawans();

			pthread_mutex_unlock(&mutex);
		}

		void aguardaTestePadawan(int id) {
			pthread_mutex_lock(&mutex);
			
			std::cout << "Padawan_" << id << " está aguardando teste" << std::endl;

			// std::cout << "Testem em andamento? " << (testeEmAndamento ? "false" : "true") << std::endl;
			while(testeEmAndamento == false){
				std::cout << std::endl << "To preso =(" << std::endl;
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}
			
			pthread_cond_broadcast(&condPadawanSalao);

			std::cout << std::endl << "COUNT: " << countEsperandoTeste << std::endl;

			std::cout << std::endl << "ENTRARAM " << this->padawansNoSalao.size() << " padawans no salao" << std::endl;

			if(!qntdTesteSetada) {
				// std::cout << std::endl << "ENTRARAM " << this->padawansNoSalao.size() << " padawans no salao" << std::endl;
				std::cout << std::endl << "Padawan_" << id << " entrou no countteste" << std::endl;
				countEmTeste = this->padawansNoSalao.size();
				qntdTesteSetada = true;
			}


			
			countEmTeste--;

			if (this->countEmTeste == 0) {
				pthread_cond_signal(&condYoda);
			}
			
			std::cout << std::endl << "Acho que escapei" << std::endl;

			pthread_mutex_unlock(&mutex);
		}

		void realizaTeste(int id) {
			pthread_mutex_lock(&mutex);

			std::cout << std::endl << "padawan_" << id << " entrou no teste" << std::endl;
			std::cout << std::endl << "Primeiro da fila: " << filaPadawans.front() << std::endl;
			
			while(filaPadawans.front() != id){
				std::cout << "Padawan_" << id << " está esperando o teste na fila" << std::endl;
				pthread_cond_wait(&condTeste, &mutex);
				
			}
			
			pthread_cond_signal(&condTeste);
			filaPadawans.pop();

			std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;
			// pthread_cond_signal(&condTeste);
			
			// pthread_cond_signal(&condYoda);

			// pthread_cond_signal(&condPadawanSalao);

			if (!this->qntdTesteSetada) {
				// std::cout << std::endl << "ENTRARAM " << this->padawansNoSalao.size() << " padawans no salao" << std::endl;
				countEmTeste = this->padawansNoSalao.size();
				qntdTesteSetada = true;
			}

			countEmTeste--;


			if(this->countEmTeste == 0) {
				// padawanEmTeste = true;
				pthread_cond_signal(&condYoda);
			}

			while (this->testeEmAndamento) {
				pthread_cond_wait(&condPadawanSalao, &mutex);
			}

			pthread_mutex_unlock(&mutex);
		}


		void aguardaCortaTranca(int id) {
			pthread_mutex_lock(&mutex);
			countEsperandoTeste++;

			

			pthread_cond_signal(&condPadawanSalao);

			if(countEsperandoTeste == (int) this->padawansAprovados.size()) {
				pthread_cond_signal(&condYoda);
			}

			std::cout << "Padawan_" << id << " aguardando corte de tranca" << std::endl;
			while (this->padawansAprovados.size() > 0) {
				pthread_cond_wait(&condCortaTranca, &mutex);
			}
			pthread_mutex_unlock(&mutex);
		}

		void cumprimentaMestresAvaliadores(int id) {
			std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;

		}
		
		void cumprimentaYoda(int id) {
			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&condPadawanSalao);
			std::cout << "Padawan_" << id << " decepcionadamente cumprimenta Yoda" << std::endl;
			pthread_mutex_unlock(&mutex);
		}

};

// Monitor Global
MonitorSalao monitorSalao(NUM_PADAWANS);


// Threads

void *yodaThread(void *arg) {
	std::cout << "Yoda foi criado" << std::endl;
	while (monitorSalao.getTotalPadawans() > 0) {
		monitorSalao.liberarEntrada();
		monitorSalao.iniciarTestes();
		monitorSalao.anunciaResultado();
		monitorSalao.cortaTranca();

	}
	monitorSalao.finalizaTestes();

	pthread_exit(NULL);
}

void *padawanThread(void *arg) {
	int id = *(int *)arg;
	delete (int *)arg;

	monitorSalao.entraSalaoPadawan(id);
	monitorSalao.aguardaTestePadawan(id);
	monitorSalao.realizaTeste(id);

	if (monitorSalao.estaAprovado(id)) {
		monitorSalao.aguardaCortaTranca(id);
		monitorSalao.saiSalaoPadawan(id, true);

		std::cout << "Padawan_" << id << " foi aprovado e vai sair do salao" << std::endl;
		pthread_exit(NULL);
	}

	monitorSalao.cumprimentaYoda(id);
	monitorSalao.saiSalaoPadawan(id);

	std::cout << "Padawan_" << id << " foi reprovado e vai sair do salao" << std::endl;
	pthread_exit(NULL);
}

void *espectadorThread(void *arg) {
	int id = *(int *)arg;
	delete (int *)arg;
	
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
		int* id = new int(i);
		std::cout << "Padawan_" << *id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	int id = i;
	// 	std::cout << "Espectador_" << id << " foi criado" << std::endl;
	// 	pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id);
	// }

	pthread_create(&yoda, NULL, yodaThread, NULL);

	
	for (int i = 0; i < numPadawans; i++) {
    	pthread_join(padawans[i], NULL);
	}

	

	// for(int i = 0; i < numEspectadores; i++) {
	// 	pthread_join(espectadores[i], NULL);
	// }

	pthread_join(yoda, NULL);

	return 0;
}