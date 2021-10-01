#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define INVALID_INPUT 1
#define BUFFER_SIZE 5

typedef long long int lli;

int P; // Numero de threads produtoras
int C; // Numero de threads consumidoras
int buffer[BUFFER_SIZE]; 
int in; // Index para produtores
sem_t fullSlots;
sem_t emptySlots;
sem_t mutex;


void initArgs(int argc, char* argv[]) {
	if (argc < 3) {
		printf("ERRO DE ENTRADA. Modo de usar:\n%s <Numero de threads consumidoras> <Numero de threads produtoras>\n", argv[0]);
		exit(INVALID_INPUT);
	}
	C = atoi(argv[1]);
	P = atoi(argv[2]);

	in = 0;

	sem_init(&fullSlots, 0, 0);
	sem_init(&emptySlots, 0, BUFFER_SIZE);
	sem_init(&mutex, 0, 1);
}

void gastaTempo() {
	for (int i = 0; i < 100000000; i++);
	for (int i = 0; i < 100000000; i++);
}

void* produtor(void* arg) {
	lli id = (lli) arg;
	
	while (1) {
		printf("Produtor %lld na fila\n", id);

		sem_wait(&emptySlots);
		sem_wait(&mutex);

		printf("Produtor %lld produzindo\n", id);
	
		gastaTempo();

		buffer[in] = id+1;
		in++;

		if (in == BUFFER_SIZE) {
			in = 0;
			sem_post(&fullSlots);
		}	

		sem_post(&mutex);
	}
}

void* consumidor(void* arg) {
	lli id = (lli) arg;
	
	while (1) {
		printf("Consumidor %lld na fila\n", id);

		sem_wait(&fullSlots);
		sem_wait(&mutex);
	
		printf("Consumidor %lld consumindo\n", id);
	
		for (int i = 0; i < BUFFER_SIZE; i++) {
			printf("%d ", buffer[i]);
			buffer[i] = 0;
		}
		printf("\n");
		sem_post(&mutex);

		for (int i = 0; i < BUFFER_SIZE; i++) {
			sem_post(&emptySlots);
		}
	}
}

pthread_t* createTid(int N) {
	pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * N);
	if (tid == NULL) {
		puts("ERRO--createTid");
		exit(-1);
	}
	return tid;
}

void createThreads(pthread_t* tidCons, pthread_t* tidProd) {
	for (lli i = 0; i < P; i++) {
		if (pthread_create(tidProd + i, NULL, produtor, (void *)i)) {
			puts("ERRO--createThreads");
			exit(-1);
		}
	}

	for (lli i = 0; i < C; i++) {
		if (pthread_create(tidCons + i, NULL, consumidor, (void *)i)) {
			puts("ERRO--createThreads");
			exit(-1);
		}
	}
}

void waitThreads(pthread_t* tid, int N) {
	for (int i = 0; i < N; i++) {
		if (pthread_join(tid[i], NULL)) {
			puts("ERRO--pthread_join");
			exit(-1);
		}
	}
}

void run() {
	pthread_t* tidCons = createTid(C);
	pthread_t* tidProd = createTid(P);

	createThreads(tidCons, tidProd);

	waitThreads(tidCons, C);
	waitThreads(tidProd, P);
}

void endArgs() {
	sem_destroy(&fullSlots);
	sem_destroy(&emptySlots);
	sem_destroy(&mutex);
}

int main(int argc, char* argv[]) {
	initArgs(argc, argv);	
	run();
	endArgs();
	return 0;
}
