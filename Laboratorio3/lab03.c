#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <float.h>
#include "timer.h"

/* DEFINIÇÕES DE VARIAVEIS */
typedef long long int lli;

typedef struct {
	float min;
	float max;
} _values;

/* VARIÁVEIS GLOBAIS */
float* array;
lli size;
int nthreads;

/* FUNÇÕES */
float min(float a, float b) {
	return a < b? a : b;
}

float max(float a, float b) {
	return a > b? a : b;
}

float randomFloat(float maxNumber) {	
	return ((float) rand())/((float)RAND_MAX) * maxNumber;
}

void allocateArray() {
	array = (float *) malloc(sizeof(float) * size);
	if (array == NULL) {
		fprintf(stderr, "ERRO--malloc");
		exit(-2);
	}
}

void initArray(float maxNumber) {
	for (lli i = 0ll; i < size; i++) 
		array[i] = randomFloat(maxNumber);
}

void init(int argc, char* argv[], float maxNumber) {
	if (argc < 3) {
		printf("Digite: %s <tamanho do vetor> <numero de threads>\n", argv[0]);
		exit(-1);
	}
	size = atoll(argv[1]);
	nthreads = atoi(argv[2]);
	nthreads = (nthreads <= size)? nthreads : size;

	allocateArray();

	srand(time(NULL));

	initArray(maxNumber);
}

_values* initValues() {
	_values* values = (_values *) malloc(sizeof(_values));
     	if (values == NULL) {
		fprintf(stderr, "ERRO--malloc values\n");
		exit(-3);
	}
	values -> min = FLT_MAX;
	values -> max = FLT_MIN;
	return values;	

}


_values* findValuesSeq() {
	_values* values = initValues();
	
	for (lli i = 0ll; i < size; i++) {
		values -> min = min(values -> min, array[i]);
		values -> max = max(values -> max, array[i]);
	}
	return values;	
}

void* findConc(void* args) {
	lli begin = (lli) args;
	_values* values = initValues();

	for (lli i = begin; i < size; i+=(lli)nthreads) {
		values -> min = min(values -> min, array[i]);
		values -> max = max(values -> max, array[i]);
	}
	
	pthread_exit((void *) values);

}

pthread_t* createTid() {
	pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
	if (tid == NULL) {
		fprintf(stderr, "ERRO--malloc\n");
		exit(-4);
	}
	return tid;
}

void createThreads(pthread_t* tid) {
	for (lli i = 0ll; i < nthreads; i++) {
		if (pthread_create(tid+i, NULL, findConc, (void *) i)) {
			fprintf(stderr, "ERRO--pthread_create\n");
			exit(-5);
		}
	}
	
}

_values* waitThreads(pthread_t* tid) {
	_values* values = initValues();
	_values* threadValues;
	for (lli i = 0; i < nthreads; i++) {
		if (pthread_join(*(tid+i), (void **) &threadValues)) {
			fprintf(stderr, "ERRO--pthread_join\n");
			exit(-6);
		}	
		values -> min = min(values -> min, threadValues -> min);
		values -> max = max(values -> max, threadValues -> max);
		free(threadValues);
	}
	free(tid);

	return values;
}


_values* findValuesConc() {
	pthread_t* tid = createTid();

	createThreads(tid);
	
	return waitThreads(tid);
}


int main(int argc, char* argv[]) {
	double begin, end, totalSeq, totalConc;
		
	float maxNumber = 10000.0;
	_values* valuesSeq,* valuesConc;

	init(argc, argv, maxNumber);

	GET_TIME(begin);
	valuesSeq = findValuesSeq();
	GET_TIME(end);
	totalSeq = end - begin;

	GET_TIME(begin);
	valuesConc = findValuesConc();
	GET_TIME(end);
	totalConc = end - begin;

	free(array);

	printf("Seq\nMax: %f\nMin: %f\n", valuesSeq -> max, valuesSeq -> min); 
	puts("=====");
	printf("Conc\nMax: %f\nMin: %f\n", valuesConc -> max, valuesConc -> min);
	puts("=====");
	printf("Total sequencial: %lf\n", totalSeq);
	printf("Total concorrente: %lf\n", totalConc);
	printf("Ganho de desempenho: %lf\n", totalSeq/totalConc);

	return 0;
}
