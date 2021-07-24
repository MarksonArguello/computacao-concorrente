#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

const int elemento = 2;
int dim;
int nthreads;
float* mat;
float* saida;

int min(int a, int b) {
        if (a < b)
                return a;
        return b;
}

void init() {
        mat   = (float *) malloc(dim * dim * sizeof(float));
        saida = (float *) malloc(dim * dim * sizeof(float));
        if (mat == NULL || saida == NULL) {
                puts("ERRO--malloc");
                exit(-1);
        }
}

void initMatriz() {
        for (int i = 0; i < dim; i++)
                for (int j = 0; j < dim; j++)
                        mat[i * dim + j] = elemento;
}

void* multiplicaMatrizes(void* arg) {
        int linha = *((int *) arg);

        for (int i = linha; i < dim; i += nthreads)
                for (int j = 0; j < dim; j++)
                        for (int k = 0; k < dim; k++)
                                saida[i * dim + j] += mat[i * dim + k] * mat[k * dim + j];

        pthread_exit(NULL);
}

void criaThreads(pthread_t tid[], int args[]) {
        for (int i = 0; i < nthreads; i++) {
                tid[i] = args[i] = i;
                if (pthread_create(&tid[i], NULL, multiplicaMatrizes,(void *) &args[i])) {
                        puts("--ERRO: pthread_create()");
                        exit(-1);
                }
        }
}

void esperaThreads(pthread_t tid[]) {
        for (int i = 0; i < nthreads; i++)
                if (pthread_join(tid[i], NULL)) {
                        puts("--ERRO: pthread_join()");
                        exit(-1);
                }
}

void verificaErro() {
        float saidaEsperada = elemento * elemento * dim;
        for (int i = 0; i < dim; i++)
                for (int j = 0; j < dim; j++)
                        if (saida[i * dim + j] != saidaEsperada) {
                                printf("Erro na linha %d coluna %d\nSaida esperadan %lf\nSaida obtida %lf\n", (i+1), (j+1), saidaEsperada, saida[i * dim + j]);
                                exit(-1);
                        }
        free(saida);
}


void inicializacao(int argc, char **argv) {
        if (argc < 3) {
                printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
                exit(-1);
        }

        dim = atoi(argv[1]);
        nthreads = atoi(argv[2]);
        nthreads = min(dim, nthreads);
        init();
        initMatriz();
}


int main(int argc, char **argv) {

        double inicio, fim, delta, tot = 0.0;

        GET_TIME(inicio)
        inicializacao(argc, argv);
        pthread_t tid[nthreads];
        int args[nthreads];
        GET_TIME(fim);

        delta = fim - inicio;
        tot += delta;
        printf("Tempo de inicializacao: %lf\n", delta);


        GET_TIME(inicio);
        criaThreads(tid, args);
        esperaThreads(tid);
        GET_TIME(fim);

        delta = fim - inicio;
        tot += delta;
        printf("Tempo de multiplicacao: %lf\n", delta);


        GET_TIME(inicio);
        free(mat);
        verificaErro();
        GET_TIME(fim);


        delta = fim - inicio;
        tot += delta;
        printf("Tempo de finalizacao %lf\n", delta);
        printf("Tempo total: %lf\n", tot);
        puts("Programa Correto");

        return 0;
}
