#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t cond_1, cond_2;

/* Thread A */
void *A (void *t) {
        pthread_mutex_lock(&x_mutex);

        while (x == 0) {
                pthread_cond_wait(&cond_1, &x_mutex);
        }
        printf("Fique a vontade.\n");
        x++;
        pthread_cond_broadcast(&cond_2);

        pthread_mutex_unlock(&x_mutex);
        pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {
        pthread_mutex_lock(&x_mutex);

        printf("Seja bem-vindo!\n");
	x++;
        pthread_cond_broadcast(&cond_1);
       

        pthread_mutex_unlock(&x_mutex);
        pthread_exit(NULL);
}

/* Thread C */
void *C (void *t) {
        pthread_mutex_lock(&x_mutex);

        while (x != 3) {
                pthread_cond_wait(&cond_2, &x_mutex);
        }
        printf("Volte sempre.\n");
        x++;

        pthread_mutex_unlock(&x_mutex);
        pthread_exit(NULL);
}

/* Thread D */
void *D (void *t) {
        pthread_mutex_lock(&x_mutex);

        while (x == 0) {
                pthread_cond_wait(&cond_1, &x_mutex);
        }
        printf("Sente-se por favor.\n");
        x++;
        pthread_cond_broadcast(&cond_2);

        pthread_mutex_unlock(&x_mutex);
        pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
        int i;
        pthread_t threads[NTHREADS];

        /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
        pthread_mutex_init(&x_mutex, NULL);
        pthread_cond_init (&cond_1, NULL);
        pthread_cond_init (&cond_2, NULL);

        /* Cria as threads */
        pthread_create(&threads[0], NULL, A, NULL);
        pthread_create(&threads[1], NULL, B, NULL);
        pthread_create(&threads[2], NULL, C, NULL);
        pthread_create(&threads[3], NULL, D, NULL);

        /* Espera todas as threads completarem */
        for (i = 0; i < NTHREADS; i++) {
                pthread_join(threads[i], NULL);
        }

        /* Desaloca variaveis e termina */
        pthread_mutex_destroy(&x_mutex);
        pthread_cond_destroy(&cond_1);
        pthread_cond_destroy(&cond_2);
        
        return 0;
}

