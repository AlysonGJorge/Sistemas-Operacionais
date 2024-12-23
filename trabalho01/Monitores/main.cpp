/*
 * FUNCIONALIDADE: Implementação de um sistema de avaliação de Padawans gerenciado por Yoda.
 * AUTORES: Natanael Aparecido Tagliaferro Galafassi, Alyson Gonçalves Jorge, Hudson Taylor Perrut Cassim.
 * DATA: 20/12/2024
 */


#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <algorithm>

#define NUM_PADAWANS 5 // Número de padawans
#define NUM_ESPECTADORES 10 // Número de espectadores

// variaveis globais
int numPadawans = NUM_PADAWANS; // Inicializa o número de padawans
int numEspectadores = NUM_ESPECTADORES; // Inicializa o número de espectadores
int totalPadawans; // Total de padawans
int totalEspectadores; // Total de espectadores
std::list<int> padawansNoSalao; // Lista de padawans no salao
std::list<int> espectadoresNoSalao; // Lista de espectadores no salao
std::list<int> padawansAprovados; // Lista de padawans aprovados
std::queue<int> filaPadawans; // Fila de padawans
bool primeiraExecucao; // Variavel de controle para a primeira execução
int countEsperandoTeste; // Contador de padawans esperando teste
int countEmTeste; // Contador de padawans em teste
bool salaoAberto; // Variavel de controle para o salao aberto
bool testesConcluidos; // Variavel de controle para os testes concluidos
bool testeEmAndamento; // Variavel de controle para o teste em andamento
bool qntdTesteSetada; /// Variavel de controle para a quantidade de padawans setadas para o teste

pthread_mutex_t mutex; // Mutex
pthread_cond_t condPadawan; // Variavel de condição para padawan
pthread_cond_t condPadawanSalao; // Variavel de condição para padawan no salao
pthread_cond_t condTeste; // Variavel de condição para padawans em teste
pthread_cond_t condEspectador; // Variavel de condição para espectador
pthread_cond_t condEspectadorSalao; // Variavel de condição para espectador no salao
pthread_cond_t condYoda; // Variavel de condição para yoda
pthread_cond_t condCortaTranca; // Variavel de condição para cortar a tranca

void cumprimentaMestresAvaliadores(int id); // Função para cumprimentar os mestres avaliadores



/*
Verifica se o padawan foi aprovado.
@params id: id do padawan
@returns bool: true se o padawan foi aprovado, false caso contrário
*/
bool estaAprovado(int id)
{
	pthread_mutex_lock(&mutex);
	bool aprovado = std::find(padawansAprovados.begin(), padawansAprovados.end(), id) != padawansAprovados.end();
	pthread_mutex_unlock(&mutex);
	return aprovado;
}

// Utilitario

/*
Faz o discurso do Yoda.
*/
void discursoYoda()
{
	std::cout << "Yoda começou a discursar" << std::endl;
	std::cout << "[Yoda]:A avaliação, concluída está. Muitos desafios enfrentaram, mas a força dentro de cada um de vocês, clara foi." << std::endl;

	std::cout << " [Yoda]: Alguns de vocês, Jedi se tornam, outros, o caminho ainda terão que percorrer. \nO sucesso, mais que habilidade demonstra, a coragem, a perseverança e a sabedoria que cada um possui em seu coração." << std::endl;

	std::cout << "[Yoda]: Lembre-se, o aprendizado nunca termina. A jornada, longa é. \nEm vocês, jovens Padawans, vejo o futuro da Ordem. Não importa o resultado, todos cresceram e aprenderam algo valioso." << std::endl;

	std::cout << "[Yoda]: A Força, sempre os guiará. Mantenham-se firmes em seu propósito, e nunca duvidem de si mesmos, pois grandes feitos estão diante de vocês. Que a Força os acompanhe, hoje e sempre." << std::endl;
}

// Yoda


/*
Função responsável por controlar a entrada de padawans e espectadores no salão.
*/
void liberarEntrada()
{
	pthread_mutex_lock(&mutex);
	int time = rand() % 10 + 1; // Tempo de espera para liberar a entrada


	std::cout << "[Yoda]: Por " << time << " segundos, a entrada liberarei" << std::endl;
	std::cout << "Yoda liberou a entrada para os próximos testes" << std::endl;
	salaoAberto = true;
	pthread_cond_broadcast(&condPadawan); // Libera a entrada dos padawans
	pthread_cond_signal(&condEspectador); // Libera a entrada dos espectadores

	pthread_mutex_unlock(&mutex);
	sleep(time); // Tempo de espera para liberar a entrada

	pthread_mutex_lock(&mutex);
	salaoAberto = false; // Fecha a entrada do salão
	primeiraExecucao = false; // Variável de controle para a primeira execução
	

	pthread_mutex_unlock(&mutex);
}


/* 
Função responsável por iniciar os testes.
*/
void iniciarTestes()
{
	pthread_mutex_lock(&mutex);

	// Só inicia os testes quando todos os padawans que entraram no salão estiverem aguardando o teste.
	while (countEsperandoTeste > 0)
	{
		pthread_cond_wait(&condYoda, &mutex);
	}

	std::cout << "[Yoda]: Em teste, todos estão. O teste começará em breve." << std::endl;
	testeEmAndamento = true; // Variável de controle para o teste em andamento
	qntdTesteSetada = false; // Variável de controle para a quantidade de padawans setadas para o teste

	pthread_cond_broadcast(&condPadawanSalao); // libera os padawans para o teste
	pthread_cond_broadcast(&condEspectadorSalao); // libera os espectadores para o teste
	pthread_mutex_unlock(&mutex);


}

/*
Função responsável por anunciar os resultados dos testes.
*/
void anunciaResultado()
{
	pthread_mutex_lock(&mutex);

	pthread_cond_broadcast(&condPadawanSalao); // libera os padawans que estão esperando o término do teste.
	
	pthread_mutex_unlock(&mutex);

	// Acorda os padawans que estão esperando na fila por sua vez.
	while (!filaPadawans.empty())
	{
		pthread_cond_signal(&condTeste);
	}


	pthread_mutex_lock(&mutex);
	
	// Garante que a continuação do fluxo só siga quando todos os padawans terem concluído o teste.
	while (countEmTeste > 0)
	{
		pthread_cond_wait(&condYoda, &mutex);
	}

	std::cout << "[Yoda]: Concluído o teste foi. Os Resultados anunciados em breve serão." << std::endl;

	// Percorre a lista de padawans que estão no salão e 'aleatoriza' se o padawan foi aprovado ou não.
	for (auto &padawan : padawansNoSalao)
	{
		int r = rand() % 100 + 1;
		if (r >= 50)
		{
			std::cout << "[Yoda]: Padawan_" << padawan << " foi aprovado" << std::endl;
			padawansAprovados.push_back(padawan); // 
		}
	}

	testeEmAndamento = false; // Variável de controle para o teste em andamento
	qntdTesteSetada = false; // Variável de controle para a quantidade de padawans setadas para o teste
	pthread_cond_broadcast(&condTeste);

	pthread_mutex_unlock(&mutex);
}


/*
Função responsável pela cerimônia de corte de tranças dos padawans aprovados.
*/
void cortaTranca()
{
	pthread_mutex_lock(&mutex);
	std::cout << "[Yoda]: A cerimônia de corte de trança começará em breve" << std::endl;


	// Garante que a continuação do fluxo só siga quando todos os padawans aprovados estiverem aguardando o corte de trança.
	while (countEsperandoTeste < (int)padawansAprovados.size())
	{
		pthread_cond_wait(&condYoda, &mutex);
	}

	// Anuncia o corte de trança para os padawans aprovados
	for (auto &padawan : padawansAprovados)
	{
		std::cout << "[Yoda]: Padawan_" << padawan << " sua trança cortada teve. " << std::endl;
	}
	
	padawansAprovados.clear(); // Limpa a lista de padawans aprovados

	pthread_cond_broadcast(&condCortaTranca); // Libera os padawans aprovados para sair do salão


	// Garante que todos os padawans saiam do salão antes de começar uma nova rotina.
	while (!primeiraExecucao && padawansNoSalao.size() > 0)
	{
		pthread_cond_wait(&condYoda, &mutex); // Espera todos os padawans sairem do salão
	}

	pthread_mutex_unlock(&mutex);
}

// Função responsável por finalizar os testes.
void finalizaTestes()
{
	pthread_mutex_lock(&mutex);
	std::cout << "[Yoda]: Por fim, os testes todos se encerram." << std::endl;
	discursoYoda();
	pthread_mutex_unlock(&mutex);
	
}

// Padawan

/*
Função responsável por controlar a entrada dos padawans no salão.
@param id: id do padawan
*/
void entraSalaoPadawan(int id)
{
	pthread_mutex_lock(&mutex);

	// Aguarda o Yoda liberar a entrada
	while (!salaoAberto)
	{
		pthread_cond_wait(&condPadawan, &mutex); // Espera o Yoda liberar a entrada
	}

	pthread_cond_signal(&condPadawan); // Libera a entrada do padawan atrás dele na fila

	padawansNoSalao.push_back(id); // Adiciona o padawan na lista de padawans no salão
	filaPadawans.push(id); // Adiciona o padawan na fila de padawans

	std::cout << "Padawan_" << id << " entrou no salao" << std::endl;
	cumprimentaMestresAvaliadores(id); // Cumprimenta os mestres avaliadores

	pthread_mutex_unlock(&mutex);
}

/*
Função responsável por controlar a saída dos padawans do salão.
@param id: id do padawan
@param aprovado: se o padawan foi aprovado ou não (por padrão é falso)
*/
void saiSalaoPadawan(int id, bool aprovado = false)
{
	pthread_mutex_lock(&mutex);

	// Não pode sair do salão enquanto o teste estiver em andamento
	while (testeEmAndamento)
	{
		pthread_cond_wait(&condPadawanSalao, &mutex);
	}

	
	if (aprovado)
	{
		pthread_cond_signal(&condCortaTranca);
	}

	std::cout << "Padawan_" << id << " saiu do salao" << std::endl;
	padawansNoSalao.remove(id); // Remove o padawan da lista de padawans no salão

	totalPadawans--; // Decrementa o total de padawans

	// Se não tiver mais padawans e espectadores no salão, acorda o Yoda para continuar sua execução
	if (padawansNoSalao.size() == 0 && espectadoresNoSalao.size() == 0)
	{
		pthread_cond_signal(&condYoda);
	}

	pthread_mutex_unlock(&mutex);

	
}

/*
Função responsável por aguardar o teste do padawan.
@param id: id do padawan
*/
void aguardaTestePadawan(int id)
{
	pthread_mutex_lock(&mutex);

	std::cout << "Padawan_" << id << " está aguardando teste" << std::endl;

	pthread_cond_wait(&condPadawanSalao, &mutex); 

	pthread_cond_signal(&condPadawanSalao);

	// Faz a variavel de controle de entrada de padawans na função receber o tamanho da lista de padawans no salão.
	if (!qntdTesteSetada)
	{
		countEmTeste = padawansNoSalao.size();
		qntdTesteSetada = true;
	}

	countEmTeste--; // Decrementa o contador de padawans em teste


	// Se todos os padawans estiverem aguardando o teste, acorda o Yoda para continuar sua execução
	if (countEmTeste == 0)
	{
		pthread_cond_signal(&condYoda);
	}

	pthread_mutex_unlock(&mutex);
}


/*
Função responsável por realizar o teste do padawan.
@param id: id do padawan
*/
void realizaTeste(int id)
{
	pthread_mutex_lock(&mutex);

	// O padawan espera ser a sua vez no teste
	while (filaPadawans.front() != id)
	{
		pthread_cond_wait(&condTeste, &mutex);
	}

	filaPadawans.pop(); // Remove o padawan da fila
	pthread_cond_signal(&condTeste); // Libera o padawan atrás dele na fila

	std::cout << "Padawan_" << id << " está realizando o teste." << std::endl;

	// Faz a variavel de controle de entrada de padawans na função receber o tamanho da lista de padawans no salão.
	if (!qntdTesteSetada)
	{
		countEmTeste = padawansNoSalao.size();
		qntdTesteSetada = true;
	}

	countEmTeste--; // Decrementa o contador de padawans em teste

	// Se todos os padawans estiverem realizado o teste, acorda o Yoda para continuar sua execução
	if (countEmTeste == 0)
	{
		pthread_cond_signal(&condYoda);
	}

	// Não pode avançar enquanto o teste estiver em andamento.
	while (testeEmAndamento)
	{
		pthread_cond_wait(&condTeste, &mutex);
	}

	pthread_mutex_unlock(&mutex);
}


/*
Função responsável por aguardar o corte de trança do padawan.
@param id: id do padawan
*/
void aguardaCortaTranca(int id)
{
	pthread_mutex_lock(&mutex);
	countEsperandoTeste++; // Incrementa o contador de padawans esperando o corte de trança

	pthread_cond_signal(&condTeste); // Libera o padawan atrás dele na fila

	// Se todos os padawans aprovados estiverem aguardando o corte de trança, acorda o Yoda para continuar
	if (countEsperandoTeste == (int)padawansAprovados.size())
	{
		pthread_cond_signal(&condYoda);
	}

	std::cout << "Padawan_" << id << " está aguardando o corte de trança" << std::endl;

	// Não pode avançar enquanto todos os padawans aprovados não tiverem sua trança cortada.
	while (padawansAprovados.size() > 0)
	{
		pthread_cond_wait(&condCortaTranca, &mutex);
	}
	pthread_mutex_unlock(&mutex);
}

/*
Função responsável por cumprimentar os mestres avaliadores.
@param id: id do padawan
*/
void cumprimentaMestresAvaliadores(int id)
{
	std::cout << "Padawan_" << id << " cumprimentou os mestres avaliadores" << std::endl;
}

/*
Função responsável por cumprimentar Yoda.
@param id: id do padawan
*/
void cumprimentaYoda(int id)
{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&condPadawanSalao); // Libera o padawan atrás dele na fila
	std::cout << "Padawan_" << id << " decepcionadamente cumprimenta Yoda" << std::endl;
	pthread_mutex_unlock(&mutex);
}

// Threads

/*
Função da thread do Yoda.
@param arg: argumento da thread
*/
void *yodaThread(void *arg)
{
	std::cout << "Yoda foi criado" << std::endl;

	// Enquanto tiver padawans ou espectadores querendo entrar no salão, o Yoda continua sua execução
	while (totalPadawans > 0 && totalEspectadores > 0)
	{
		liberarEntrada(); // Libera a entrada dos padawans e espectadores
		iniciarTestes(); // Inicia os testes
		anunciaResultado(); // Anuncia os resultados
		cortaTranca(); // Corta a trança dos padawans aprovados
	}
	finalizaTestes(); // Finaliza os testes quando não tiver mais padawans ou espectadores para entrar no salão.

	pthread_exit(NULL);
}

/*
Função da thread do padawan.
@param arg: argumento da thread (vai ser usado como id)
*/
void *padawanThread(void *arg)
{
	int id = *(int *)arg; // id do padawan
	delete (int *)arg; // deleta o argumento

	entraSalaoPadawan(id); // Padawan entra no salão
	aguardaTestePadawan(id); // Padawan aguarda o teste
	realizaTeste(id); // Padawan realiza o teste

	// Se o padawan foi aprovado, ele aguarda o corte de trança, caso contrário, ele cumprimenta o Yoda e sai do salão
	if (estaAprovado(id))
	{
		aguardaCortaTranca(id);
		saiSalaoPadawan(id, true);

		pthread_exit(NULL);
	}

	cumprimentaYoda(id); // Cumprimenta Yoda
	saiSalaoPadawan(id); // Padawan sai do salão

	pthread_exit(NULL);
}


/*
Função responsável por controlar a entrada dos espectadores no salão.
@param id: id do espectador
*/
void entraSalaoEspectador(int id)
{
	pthread_mutex_lock(&mutex);

	// Aguarda o Yoda liberar a entrada
	while (!salaoAberto)
	{
		pthread_cond_wait(&condEspectador, &mutex); // Espera o Yoda liberar a entrada
	}

	pthread_cond_signal(&condEspectador); // Libera a entrada do espectador atrás dele na fila
	espectadoresNoSalao.push_back(id); // Adiciona o espectador na lista de espectadores no salão
	std::cout << "Espectador_" << id << " entrou no salao" << std::endl;
	pthread_mutex_unlock(&mutex);
}


/*
Função responsável por controlar a saída dos espectadores do salão.
@param id: id do espectador
*/
void saiSalaoEspectador(int id)
{

	std::cout << "Espectador_" << id << " se entediou e resolve sair do salao" << std::endl;
	
	espectadoresNoSalao.remove(id); // Remove o espectador da lista de espectadores no salão

	totalEspectadores--;  // Decrementa o total de espectadores

	// Se não tiver mais padawans e espectadores no salão, acorda o Yoda para continuar sua execução.
	if (espectadoresNoSalao.size() == 0 && padawansNoSalao.size() == 0)
	{
		// acorda yoda para poder finalizar os testes
		pthread_cond_signal(&condYoda);
	}
	
}

/*
Função responsável por assistir os testes.
@param id: id do espectador
*/
void assiste_testes(int id)
{
	pthread_cond_wait(&condEspectadorSalao, &mutex); // Aguarda o teste começar
	int tempo = rand() % 10 + 1; // Tempo de espera para assistir os testes

	std::cout << "Espectador_" << id << " está assistindo os testes" << std::endl;
	pthread_cond_signal(&condEspectadorSalao); // Libera o espectador atrás dele na fila
	pthread_mutex_unlock(&mutex);
	sleep(tempo); // Tempo de espera para assistir os testes

}


/*
Função da thread do espectador.
@param arg: argumento da thread (vai ser usado como id)
*/
void *espectadorThread(void *arg)
{
	int id = *(int *)arg; // id do espectador
	delete (int *)arg; // deleta o argumento

	entraSalaoEspectador(id); // Espectador entra no salão
	assiste_testes(id); // Espectador assiste os testes
	saiSalaoEspectador(id); // Espectador sai do salão

	pthread_exit(NULL);

}

/*
 * Função principal.
*/
int main(int argc, char **argv)
{
	pthread_t yoda; // Thread do Yoda

	std::cout << "Digite o numero de padawans: ";
	std::cin >> numPadawans; // Pega o número de padawans
	std::cout << "Digite o numero de espectadores: ";
	std::cin >> numEspectadores; // Pega o número de espectadores
	totalPadawans = numPadawans; // Inicializa o total de padawans
	totalEspectadores = numEspectadores; // Inicializa o total de espectadores
	pthread_t padawans[numPadawans]; // Threads dos padawans
	pthread_t espectadores[numEspectadores]; // Threads dos espectadores

	pthread_mutex_init(&mutex, NULL); // Inicializa o mutex
	pthread_cond_init(&condPadawan, NULL); // Inicializa a variável de condição para padawan
	pthread_cond_init(&condPadawanSalao, NULL); // Inicializa a variável de condição para padawan no salão
	pthread_cond_init(&condTeste, NULL); // Inicializa a variável de condição para teste
	pthread_cond_init(&condEspectador, NULL); // Inicializa a variável de condição para espectador
	pthread_cond_init(&condEspectadorSalao, NULL); // Inicializa a variável de condição para espectador no salão
	pthread_cond_init(&condYoda, NULL); // Inicializa a variável de condição para yoda
	pthread_cond_init(&condCortaTranca, NULL); // Inicializa a variável de condição para cortar a trança

	// Inicializa as threads dos padawans
	for (int i = 0; i < numPadawans; i++)
	{
		int *id = new int(i); // Aloca o id do padawan
		std::cout << "Padawan_" << *id << " foi criado" << std::endl;
		pthread_create(&padawans[i], NULL, padawanThread, (void *)id); // Cria a thread do padawan
	}

	// Inicializa as threads dos espectadores
	for(int i = 0; i < numEspectadores; i++) {
		int *id = new int(i); // Aloca o id do espectador
		std::cout << "Espectador_" << *id << " foi criado" << std::endl; 
		pthread_create(&espectadores[i], NULL, espectadorThread, (void *)id); // Cria a thread do espectador
	}

	pthread_create(&yoda, NULL, yodaThread, NULL); // Cria a thread do Yoda

	// Aguarda as threads dos padawans terminarem
	for (int i = 0; i < numPadawans; i++)
	{
		pthread_join(padawans[i], NULL); // Aguarda a thread do padawan
	}

	// Aguarda as threads dos espectadores terminarem
	for(int i = 0; i < numEspectadores; i++) {
		pthread_join(espectadores[i], NULL); // Aguarda a thread do espectador
	}

	pthread_join(yoda, NULL); // Aguarda a thread do Yoda terminar.

	pthread_mutex_destroy(&mutex); // Destrói o mutex
	pthread_cond_destroy(&condPadawan); // Destrói a variável de condição para padawan
	pthread_cond_destroy(&condPadawanSalao); // Destrói a variável de condição para padawan no salão
	pthread_cond_destroy(&condTeste); // Destrói a variável de condição para teste
	pthread_cond_destroy(&condEspectador); // Destrói a variável de condição para espectador
	pthread_cond_destroy(&condEspectadorSalao); // Destrói a variável de condição para espectador no salão 
	pthread_cond_destroy(&condYoda); // Destrói a variável de condição para yoda
	pthread_cond_destroy(&condCortaTranca); // Destrói a variável de condição para cortar a trança

	return 0;
}