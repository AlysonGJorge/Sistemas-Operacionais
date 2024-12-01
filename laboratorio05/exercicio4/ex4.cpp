/*
* Exercício 4: Implemente um programa que realize a soma de dois vetores de inteiros de tamanho n.
* O programa desenvolvido cria n processos filhos, onde cada processo filho é responsável por somar uma parte dos vetores de tamanho m. 
* Meio: 
Os vetores são alocados em memória compartilhada de forma que todos os processos possam acessá-los. O programa recebe como entrada o tamanho n dos vetores e o número de processos a serem criados. Os processos filhos recebem o intervalo no qual devem realizar a soma dos vetores. O processo pai é responsável por criar os processos filhos e enviar o intervalo para cada um deles. O processo pai aguarda o término dos processos filhos e imprime o vetor resultante da soma dos vetores.
* @author Natanael Aparecido Tagliaferro Galafassi, Alysson Gonçalves Jorge, Hudson Taylor Perrut Cassim.
* @date 30/11/2024
*/




#include <iostream>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>

using namespace std;

/*
* Função para imprimir um vetor.
* @param vetor Vetor a ser impresso.
* @param nElem Número de elementos do vetor.
* @param name Nome do vetor.
*/
void printVector(int *vetor, int nElem, const char* name){
	cout << name << ": [";
	for (int i = 0; i < nElem; i++){
		cout << vetor[i];
		if (i < nElem - 1){
			cout << ", ";
		}
	}
	cout << "]" << endl;
}
 
/*
* Função para criar e mapear um objeto de memória compartilhada.
* @param fd Descritor do objeto de memória compartilhada.
* @param name Nome do objeto de memória compartilhada.
* @param size Tamanho do objeto de memória compartilhada.
* @return Ponteiro para o objeto de memória compartilhada.
* @example int *vetor = (int*)mmapper(shmFd, name, nElem*sizeof(int));
*/
void *mmapper(int fd, const char* name, int size) {
	fd = shm_open(name, O_CREAT | O_RDWR, 0666); // Cria o objeto de memória compartilhada.
	if (fd == -1) { // Verifica se houve erro.
		cerr << "Erro ao criar o objeto de memória compartilhada." 
		<< strerror(errno) << endl;
		exit(-1);
	}

	ftruncate(fd, size); // Define o tamanho do objeto de memória compartilhada.

	// Mapeia o objeto de memória compartilhada.
	void *ptr = (int*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
	if (ptr == MAP_FAILED) {
		cerr << "Erro ao mapear o objeto de memória compartilhada." << endl;
		exit(-1);
	}
	return ptr;
}


int main(int argc, char** argv){
	int nElem, nProc; // Número de elementos do vetor e número de processos.
	// Nome dos vetores.
	const char *name1 = "vetor1";
	const char *name2 = "vetor2";
	const char *name3 = "resultado";
	// Descritores dos vetores.
	int shmFd1, shmFd2, shmFd3;

	cout << "Informe o número de elementos do vetor: ";
	cin >> nElem;
	cout << "Informe os número dos processos: ";
	cin >> nProc;
	cout << endl;

	// Criando pipes.
	int pipes[nProc][2];

	for (int i = 0; i < nProc; i++){
		if (pipe(pipes[i]) == -1) { // Cria um pipe e verifica se houve erro.
			cerr << "Erro ao criar o pipe para o processo " << i << ": " 
			<< strerror(errno) << endl;
			exit(EXIT_FAILURE);
			return -1;
		}
	}

	// Criando e mapeando o primeiro vetor.
	int *vetor1 = (int*)mmapper(shmFd1, name1, nElem*sizeof(int));

	// Criando e mapeando o segundo vetor.
	int *vetor2 = (int*)mmapper(shmFd2, name2, nElem*sizeof(int));

	// Criando e mapeando o vetor de resultados.
	int *vetorR = (int*)mmapper(shmFd3, name3, nElem*sizeof(int));

	// Inicializando os vetores.
	srand(time(nullptr)); // Inicializa a semente do gerador de números aleatórios.
	for (int i = 0; i < nElem; i++){
		vetor1[i] = rand() % 101; // Números aleatórios de 0 a 100.
		vetor2[i] = rand() % 101; // Números aleatórios de 0 a 100.
		vetorR[i] = 0;
	}

	// Criando processos filhos.
	for (int i = 0; i < nProc; i++){
		pid_t pid = fork(); // Cria um processo filho.

		// Verifica se houve erro ao criar o processo filho.
		if (pid < 0) {
			cerr << "Erro ao criar o processo filho " << i << ": "
			<< strerror(errno) << endl;
			exit(EXIT_FAILURE); 
			return 1;
		} else if (pid == 0) {
			// Código do processo filho
			close(pipes[i][1]); // Fecha o descritor de escrita do pipe.

			// instanciando as variáveis de intervalo localmente.s
			int start; 
			int end;

			// Recebe o intervalo do pai.
			read(pipes[i][0], &start, sizeof(int));
			read(pipes[i][0], &end, sizeof(int));
			close(pipes[i][0]); // Fecha o descritor de leitura do pipe.

			// O filho realiza a soma dos vetores nos intervalos determinados.
			for (int j = start; j < end; j++){
				vetorR[j] = vetor1[j] + vetor2[j];
			}

			exit(0); // Encerra o processo filho.
		}
	}

	// Código do processo pai.
	for (int i =0; i < nProc; i++){

		// Calcula o intervalo de cada filho.
		// O cáculo é feito da seguinte forma: o início recebe o índice do filho multiplicado pelo número de elementos dividido pelo número de processos. O fim recebe o início mais o número de elementos dividido pelo número de processos. Se o filho for o último, o fim recebe o número de elementos. 
		int start = i * (nElem/nProc);
		int end = (i == nProc - 1) ? nElem : start + (nElem/nProc);

		// Envia o intervalo para o filho.
		close(pipes[i][0]); // Fecha o descritor de leitura do pipe.
		write(pipes[i][1], &start, sizeof(int));
		write(pipes[i][1], &end, sizeof(int));
		close(pipes[i][1]); // Fecha o descritor de escrita do pipe.
	}

	// Aguarda o termino dos processos filhos.
	for (int i = 0; i < nProc; i++){
		int waitP = waitpid(-1, NULL, 0);
		if (waitP == -1){
			cerr << "Erro ao esperar o processo filho." << strerror(errno) << endl;
			return 1;
		}
	}

	cout << "Vetores: " << endl;
	// Imprime os vetores (dependendo do tamanho dos vetores, você pode comentar esse bloco).
	printVector(vetor1, nElem, "Vetor 1");
	printVector(vetor2, nElem, "Vetor 2");
	cout << endl;

	cout << "Resultado da soma dos vetores:" << endl;
	
	// Imprime o resultado da soma dos vetores.
	printVector(vetorR, nElem, "Resultado");

	// Desalocando a memória compartilhada.
	shm_unlink(name1);
	shm_unlink(name2);
	shm_unlink(name3);

	return 0;
}