#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <algorithm>

#define NUM_PADAWANS 5
#define NUM_ESPECTADORES 10

// variaveis globais
int numPadawans = NUM_PADAWANS;
int numEspectadores = NUM_ESPECTADORES;
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

pthread_mutex_t mutex;
pthread_cond_t condPadawan;
pthread_cond_t condPadawanSalao;
pthread_cond_t condTeste;
pthread_cond_t condEspectador;
pthread_cond_t condYoda;
pthread_cond_t condCortaTranca;

void cumprimentaMestresAvaliadores(int id);

bool estaAprovado(int id)
{
	pthread_mutex_lock(&mutex);
	bool aprovado = std::find(padawansAprovados.begin(), padawansAprovados.end(), id) != padawansAprovados.end();
	pthread_mutex_unlock(&mutex);
	return aprovado;
}

// Utilitario
void discursoYoda()
{
	std::cout << "Yoda começou a discursar" << std::endl;
	std::cout << "[Yoda]:A avaliação, concluída está. Muitos desafios enfrentaram, mas a força dentro de cada um de vocês, clara foi." << std::endl;

	std::cout << " [Yoda]: Alguns de vocês, Jedi se tornam, outros, o caminho ainda terão que percorrer. \nO sucesso, mais que habilidade demonstra, a coragem, a perseverança e a sabedoria que cada um possui em seu coração." << std::endl;

	std::cout << "[Yoda]: Lembre-se, o aprendizado nunca termina. A jornada, longa é. \nEm vocês, jovens Padawans, vejo o futuro da Ordem. Não importa o resultado, todos cresceram e aprenderam algo valioso." << std::endl;

	std::cout << "[Yoda]: A Força, sempre os guiará. Mantenham-se firmes em seu propósito, e nunca duvidem de si mesmos, pois grandes feitos estão diante de vocês. Que a Força os acompanhe, hoje e sempre." << std::endl;
}

// Yoda

void liberarEntrada()
{
	pthread_mutex_lock(&mutex);
	int time = rand() % 10 + 1;

	// std::cout << "Total de padawans: " << totalPadawans << std::endl;

	

	std::cout << "[Yoda]: Por " << time << " segundos, a entrada liberarei" << std::endl;
	std::cout << "Yoda liberou a entrada para os próximos testes" << std::endl;
	salaoAberto = true;
	pthread_cond_broadcast(&condPadawan);
	pthread_cond_signal(&condEspectador);

	pthread_mutex_unlock(&mutex);
	sleep(time);

	pthread_mutex_lock(&mutex);
	salaoAberto = false;
	primeiraExecucao = false;

	// pthread_cond_signal(&condPadawanSalao);

	

	pthread_mutex_unlock(&mutex);
}

void iniciarTestes()
{
	// pthread_mutex_lock(&mutex);

	while (countEsperandoTeste > 0)
	{
		pthread_cond_wait(&condYoda, &mutex);
	}

	std::cout << "[Yoda]: Em teste, todos estão. O teste começará em breve." << std::endl;
	testeEmAndamento = true;
	qntdTesteSetada = false;
	// std::cout << "To liberando os padawansalao" << std::endl;
	pthread_cond_broadcast(&condPadawanSalao);
	// pthread_mutex_unlock(&mutex);


}

void anunciaResultado()
{
	pthread_mutex_lock(&mutex);

	pthread_cond_broadcast(&condPadawanSalao);
	

	// std::cout << std::endl << "Tamanho da fila " << padawansNoSalao.size() << std::endl;
	// std::cout << std::endl << "A fila está vazia? " << (filaPadawans.empty() ? "true" : "false") << std::endl;
	// bool fuc = false;
	pthread_mutex_unlock(&mutex);
	while (!filaPadawans.empty())
	{
		// if (!fuc)
		// {
		// 	// std::cout << std::endl << "Estou tentando acordar os padawans que estão na fila" << std::endl;
		// 	fuc = true;
		// 	std::cout << "Padawan_" << filaPadawans.front() << " acordado" << std::endl;
		// 	pthread_cond_signal(&condPadawanSalao);
		// 	std::cout << std::endl
		// 			  << "Yoda esta preso " << std::endl;
					  
			
		// }
		pthread_cond_signal(&condTeste);

	}
	pthread_mutex_lock(&mutex);
	// std::cout << std::endl
	// 		  << "Primeiro da fila: " << filaPadawans.front() << std::endl;

	while (countEmTeste > 0)
	{
		// std::cout << std::endl << "Estou esperando todos os padawans estarem realizando os teste " << std::endl;
		pthread_cond_wait(&condYoda, &mutex);
	}

	std::cout << "[Yoda]: Concluído o teste foi. Os Resultados anunciados em breve serão." << std::endl;

	for (auto &padawan : padawansNoSalao)
	{
		int r = rand() % 100 + 1;
		if (r >= 50)
		{
			std::cout << "[Yoda]: Padawan_" << padawan << " foi aprovado" << std::endl;
			padawansAprovados.push_back(padawan);
		}
	}

	testeEmAndamento = false;
	qntdTesteSetada = false;
	pthread_cond_broadcast(&condTeste);

	pthread_mutex_unlock(&mutex);
}

void cortaTranca()
{
	pthread_mutex_lock(&mutex);
	std::cout << "[Yoda]: A cerimônia de corte de trança começará em breve" << std::endl;

	while (countEsperandoTeste < (int)padawansAprovados.size())
	{
		pthread_cond_wait(&condYoda, &mutex);
	}

	for (auto &padawan : padawansAprovados)
	{
		std::cout << "[Yoda]: Padawan_" << padawan << " sua trança cortada teve. " << std::endl;
	}

	padawansAprovados.clear();

	pthread_cond_broadcast(&condCortaTranca);

	while (!primeiraExecucao && padawansNoSalao.size() > 0)
	{
		// std::cout << "Fiquei preso nu final =(" << std::endl;
		pthread_cond_wait(&condYoda, &mutex);
	}

	pthread_mutex_unlock(&mutex);
}

void finalizaTestes()
{
	pthread_mutex_lock(&mutex);
	std::cout << "[Yoda]: Por fim, os testes todos se encerram." << std::endl;
	discursoYoda();
	pthread_mutex_unlock(&mutex);
	
	// std::cout << "Escapei do final =)" << std::endl;
}

// Padawan

void entraSalaoPadawan(int id)
{
	pthread_mutex_lock(&mutex);
	while (!salaoAberto)
	{
		pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
	}

	pthread_cond_signal(&condPadawan);
	padawansNoSalao.push_back(id);
	filaPadawans.push(id);
	std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
	cumprimentaMestresAvaliadores(id);

	pthread_mutex_unlock(&mutex);
}

void saiSalaoPadawan(int id, bool aprovado = false)
{
	pthread_mutex_lock(&mutex);

	while (testeEmAndamento)
	{
		pthread_cond_wait(&condPadawanSalao, &mutex);
	}

	if (aprovado)
	{
		pthread_cond_signal(&condCortaTranca);
	}

	std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
	// std::cout << std::endl << "Tamanho da lista de padawans no salao: " << padawansNoSalao.size() << std::endl;
	padawansNoSalao.remove(id);

	totalPadawans--;

	if (padawansNoSalao.size() == 0)
	{
		// std::cout << std::endl << "OVO ACORDAR O YODA" << std::endl;
		pthread_cond_signal(&condYoda);
	}

	pthread_mutex_unlock(&mutex);

	
}

void aguardaTestePadawan(int id)
{
	pthread_mutex_lock(&mutex);

	std::cout << "Padawan_" << id << " está aguardando teste" << std::endl;

	// std::cout << "Testem em andamento? " << (testeEmAndamento ? "false" : "true") << std::endl;

		// std::cout << std::endl << "To preso =(" << std::endl;
		pthread_cond_wait(&condPadawanSalao, &mutex);
		// std::cout << std::endl << "Sai da cadeia =)" << std::endl;


	pthread_cond_signal(&condPadawanSalao);

	// std::cout << std::endl
	// 		  << "COUNT: " << countEsperandoTeste << std::endl;

	// std::cout << std::endl
	// 		  << "ENTRARAM " << padawansNoSalao.size() << " padawans no salao" << std::endl;

	if (!qntdTesteSetada)
	{
		// std::cout << std::endl << "ENTRARAM " << padawansNoSalao.size() << " padawans no salao" << std::endl;
		// std::cout << std::endl
		// 		  << "Padawan_" << id << " entrou no countteste" << std::endl;
		countEmTeste = padawansNoSalao.size();
		qntdTesteSetada = true;
	}

	countEmTeste--;

	if (countEmTeste == 0)
	{
		pthread_cond_signal(&condYoda);
	}

	// std::cout << std::endl
	// 		  << "Acho que escapei" << std::endl;

	pthread_mutex_unlock(&mutex);
}

void realizaTeste(int id)
{
	pthread_mutex_lock(&mutex);

	// std::cout << std::endl
	// 		  << "padawan_" << id << " entrou no teste" << std::endl;
	// std::cout << std::endl
	// 		  << "Primeiro da fila: " << filaPadawans.front() << std::endl;

	while (filaPadawans.front() != id)
	{
		// std::cout << "Padawan_" << id << " está esperando o teste na fila" << std::endl;
		pthread_cond_wait(&condTeste, &mutex);
	}

	filaPadawans.pop();
	pthread_cond_signal(&condTeste);

	std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;
	// pthread_cond_signal(&condTeste);

	// pthread_cond_signal(&condYoda);

	// pthread_cond_signal(&condPadawanSalao);

	if (!qntdTesteSetada)
	{
		// std::cout << std::endl << "ENTRARAM " << padawansNoSalao.size() << " padawans no salao" << std::endl;
		countEmTeste = padawansNoSalao.size();
		qntdTesteSetada = true;
	}

	countEmTeste--;

	if (countEmTeste == 0)
	{
		// padawanEmTeste = true;
		pthread_cond_signal(&condYoda);
	}

	while (testeEmAndamento)
	{
		pthread_cond_wait(&condTeste, &mutex);
	}

	pthread_mutex_unlock(&mutex);
}

void aguardaCortaTranca(int id)
{
	pthread_mutex_lock(&mutex);
	countEsperandoTeste++;

	pthread_cond_signal(&condTeste);

	if (countEsperandoTeste == (int)padawansAprovados.size())
	{
		pthread_cond_signal(&condYoda);
	}

	std::cout << "Padawan_" << id << " está aguardando o corte de trança" << std::endl;
	while (padawansAprovados.size() > 0)
	{
		pthread_cond_wait(&condCortaTranca, &mutex);
	}
	pthread_mutex_unlock(&mutex);
}

void cumprimentaMestresAvaliadores(int id)
{
	std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;
}

void cumprimentaYoda(int id)
{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&condPadawanSalao);
	std::cout << "Padawan_" << id << " decepcionadamente cumprimenta Yoda" << std::endl;
	pthread_mutex_unlock(&mutex);
}

// Threads

void *yodaThread(void *arg)
{
	std::cout << "Yoda foi criado" << std::endl;
	// std::cout << "Total de padawans: " << numPadawans << std::endl;
	while (totalPadawans > 0)
	{
		liberarEntrada();
		iniciarTestes();
		anunciaResultado();
		cortaTranca();
	}
	finalizaTestes();

	pthread_exit(NULL);
}

void *padawanThread(void *arg)
{
	int id = *(int *)arg;
	delete (int *)arg;

	entraSalaoPadawan(id);
	aguardaTestePadawan(id);
	realizaTeste(id);

	if (estaAprovado(id))
	{
		aguardaCortaTranca(id);
		saiSalaoPadawan(id, true);

		// std::cout << "Padawan_" << id << " foi aprovado e vai sair do salao" << std::endl;
		pthread_exit(NULL);
	}

	cumprimentaYoda(id);
	saiSalaoPadawan(id);

	// std::cout << "Padawan_" << id << " foi reprovado e vai sair do salao" << std::endl;
	pthread_exit(NULL);
}

void *espectadorThread(void *arg)
{
	int id = *(int *)arg;
	delete (int *)arg;

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t yoda;

	std::cout << "Digite o numero de padawans: ";
	std::cin >> numPadawans;
	// std::cout << "Digite o numero de espectadores: ";
	// std::cin >> numEspectadores;
	totalPadawans = numPadawans;
	pthread_t padawans[numPadawans];
	// pthread_t espectadores[numEspectadores];

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condPadawan, NULL);
	pthread_cond_init(&condPadawanSalao, NULL);
	pthread_cond_init(&condTeste, NULL);
	pthread_cond_init(&condEspectador, NULL);
	pthread_cond_init(&condYoda, NULL);
	pthread_cond_init(&condCortaTranca, NULL);

	for (int i = 0; i < numPadawans; i++)
	{
		int *id = new int(i);
		std::cout << "Padawan_" << *id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	int id = i;
	// 	std::cout << "Espectador_" << id << " foi criado" << std::endl;
	// 	pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id);
	// }

	pthread_create(&yoda, NULL, yodaThread, NULL);

	for (int i = 0; i < numPadawans; i++)
	{
		pthread_join(padawans[i], NULL);
	}

	// for(int i = 0; i < numEspectadores; i++) {
	// 	pthread_join(espectadores[i], NULL);
	// }

	pthread_join(yoda, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condPadawan);
	pthread_cond_destroy(&condPadawanSalao);
	pthread_cond_destroy(&condTeste);
	pthread_cond_destroy(&condEspectador);
	pthread_cond_destroy(&condYoda);
	pthread_cond_destroy(&condCortaTranca);

	return 0;
}