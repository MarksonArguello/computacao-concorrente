#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define lli long long int

/* Variaveis globais */
int bloqueadas = 0;
int N;
int* vetor;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

int numeroAleatorio() {
    return rand();
}

//funcao barreira
void barreira(int nthreads, int id) {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (nthreads-1)) {
      //ultima thread a chegar na barreira
      printf("thread %d DESBLOQUEANDO todas\n\n", id);
      pthread_cond_broadcast(&x_cond);
      bloqueadas=0;
    } else {
      printf("thread %d se bloqueando\n", id);
      bloqueadas++;
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

//funcao das threads
void *tarefa (void *arg) {
  int id = *(int*)arg;
  lli somatorio = 0;

  for (int passo=0; passo < N; passo++) {
    printf("thread %d passo %d somando\n", id, passo);
    for (int indice = 0; indice < N; indice++) {
        somatorio += vetor[indice];
    }
    //sincronizacao condicional
    barreira(N, id);

    printf("thread %d passo %d escrevendo\n", id, passo);
    vetor[id] = numeroAleatorio() % 10;

    //sincronizacao condicional
    barreira(N, id);
  }
  pthread_exit((void *) somatorio);
}

int* alocarVetor() {
    int* v = (int *) malloc(sizeof(int) * N);

    if (v == NULL) {
        printf("Erro ao alocar memória\n");
        exit(-2);
    }

    for (int i = 0; i < N; i++)
        v[i] = numeroAleatorio() % 10;

    return v;
}

void initialization(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        exit(-1);
    }
    N = atoi(argv[1]);
    vetor = alocarVetor();
}


/* Funcao principal */
int main(int argc, char *argv[]) {
  srand(time(NULL));

  initialization(argc, argv);

  pthread_t threads[N];
  int id[N];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Cria as threads */
  for(int i=0;i<N;i++) {
     id[i]=i;
     if (pthread_create(&threads[i], NULL, tarefa, (void *) &id[i])) {
        printf("ERRO--pthread_create\n");
        exit(-4);
     }
  }

  /* Espera todas as threads completarem */
  int iguais = 1;
  lli* total = (lli *) malloc(sizeof(lli));
  for (int i = 0; i < N; i++) {
    lli* totalThread = (lli *) malloc(sizeof(lli));
    if (pthread_join(threads[i],(void **) totalThread)) {
        printf("ERRO--pthread_join\n");
        exit(-5);
    }
    
    printf("thread %d soma total: %lld\n", i, *totalThread);

    if (i == 0)
        *total = *totalThread;
        
    if (*total != *totalThread) {
        iguais = 0;
    }
    
  }
  if (iguais == 1) {
    printf("As somas estão iguais\n");
    printf("Soma: %lld\n", *total);
  } else {
    printf("As somas são diferentes\n");
  }
  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  return 0;
}
