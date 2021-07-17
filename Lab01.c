/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Implementar um programa concorrente, com duas threads (alem da thread principal),
para elevar ao quadrado cada elemento de um vetor de 10000 elementos.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2
#define TAM 10000
#define ELEMENTO 3

void * elevaAoQuadrado (void * arg);
int verificaErro();

int vetor[TAM];

typedef struct {
    int inicio;
    int fim;
} t_Args;

int main(void) {
    pthread_t tid_sistema[NTHREADS];
    t_Args *arg = (t_Args *) malloc(NTHREADS * sizeof(t_Args));

    if (arg == NULL) {
         printf("--ERRO: malloc()\n"); exit(-1);
        exit(-1);
    }

    int intervalo = TAM / NTHREADS;

    for (int i = 0; i < TAM; i++)
        vetor[i] = ELEMENTO;

    for (int i = 0; i < NTHREADS; i++) {
        (arg + i) -> inicio = intervalo * i;
        (arg + i) -> inicio = intervalo * (i+1);


        if (pthread_create(&tid_sistema[i], NULL, elevaAoQuadrado, (void *) (arg + i))) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }




    for (int thread = 0; thread < NTHREADS; thread++) {
        if (pthread_join(tid_sistema[thread], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1);
        }
    }

    free(arg);

    int erro = verificaErro();
    if (erro)
        printf("Vetor na posicao %d nao esta elevado ao quadrado\n", erro);
    else
         printf("Todos os elementos foram elevados ao quadrado com sucesso.\n");



    return 0;
}

void * elevaAoQuadrado (void * arg) {
    t_Args *args = (t_Args *) arg;
    for (int i = args -> inicio; i < args -> fim; i++) {
        vetor[i] *= vetor[i];
    }
    pthread_exit(NULL);
}

int verificaErro() {
    for (int i = 0; i < TAM; i++) {
        if (vetor[i] != ELEMENTO * ELEMENTO) {
            return i;
        }
    }
    return 0;
}
