#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>


int max;
int loops;
int *buffer1,*buffer2;

int consome1  = 0;
int produz1 = 0;
int consome2  = 0;
int produz2= 0;


#define MAX (10)
int consumidores = 1;
int produtores = 1;
int nambos = 1;

void produz(int valor,int buf) {
   if(buf==1){
      buffer1[produz1] = valor;
      produz1 = (produz1+1) % max;
   }else{
      buffer2[produz2] = valor;
      produz2 = (produz2+1) % max;
   }
}

int consome(int buf) {
   int tmp; 
   if(buf==1){
      tmp = buffer1[consome1];
      consome1 = (consome1+1) %max;
   }else{
      tmp = buffer2[consome2];
      consome2 = (consome2+1) %max;
   }

   return tmp;
}

void *produtor(void *arg) {
   int i;
   for (i = 0; i < loops; i++) {
      produz(i,1);
      printf("Produtor %lld produziu %d\n em 1", (long long int) arg, i);
   }
   printf("Produtor %d finalizado\n",*((int*)arg));
   return NULL;
}

void *consumidor(void *arg) {
   int tmp = 0;
   while (1) {
      tmp = consome(2);
      printf("Consumidor %lld consumiu %d de 2\n", (long long int) arg, tmp);
   }
   return NULL;
}

void *ambos(void *arg) {
   int tmp = 0;
   while (1) {
      tmp = consome(1);
      printf("Ambos %lld consumiu %d em 1\n", (long long int) arg, tmp);
      produz(tmp,2);
      printf("Ambos %lld produziu %d em 2\n", (long long int) arg, tmp);
   }
   return NULL;
}

int main(int argc, char *argv[]) {
   if (argc != 6) {
      fprintf(stderr, "uso: %s <tambuffer> <loops> <produtores> <consumidores> <ambos>\n", argv[0]);
      exit(1);
   }
   max   = atoi(argv[1]);
   loops = atoi(argv[2]);
   produtores = atoi(argv[3]);
   consumidores = atoi(argv[4]);
   nambos = atoi(argv[5]);
   assert(consumidores <= MAX);

   buffer1 = (int *) malloc(max * sizeof(int));
   buffer2 = (int *) malloc(max * sizeof(int));
   int i;
   for (i = 0; i < max; i++) {
      buffer1[i] = 0;
      buffer2[i] = 0;
   }


   pthread_t pid[MAX], cid[MAX],aid[MAX];
   for (i = 0; i < consumidores; i++) {
      pthread_create(&cid[i], NULL, consumidor, (void *) (long long int) i); 
   }
   for (i = 0; i < nambos; i++) {
      pthread_create(&aid[i], NULL, ambos, NULL);
   }
   for (i = 0; i < produtores; i++) {
      pthread_create(&pid[i], NULL, produtor, NULL);
   }
   for (i = 0; i < consumidores; i++) {
      pthread_join(pid[i], NULL); 
      pthread_join(aid[i], NULL); 
      pthread_join(cid[i], NULL); 
   }
   return 0;
}


