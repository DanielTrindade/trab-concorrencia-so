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
int count1 = 0;
int count2 = 0;

int consumidores = 1;
int produtores = 1;
int nambos = 1;
int produtores_restantes = 0;
int ambos_restantes = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty2 = PTHREAD_COND_INITIALIZER;

void produz(int valor,int buf) {
   if(buf==1){
      buffer1[produz1] = valor;
      produz1 = (produz1+1) % max;
      count1++;
      pthread_cond_signal(&not_empty1);
   }else{
      buffer2[produz2] = valor;
      produz2 = (produz2+1) % max;
      count2++;
      pthread_cond_signal(&not_empty2);
   }
}

int consome(int buf) {
   int tmp;
   if(buf==1){
      tmp = buffer1[consome1];
      consome1 = (consome1+1) %max;
      count1--;
      pthread_cond_signal(&not_full1);
   }else{
      tmp = buffer2[consome2];
      consome2 = (consome2+1) %max;
      count2--;
      pthread_cond_signal(&not_full2);
   }

   return tmp;
}

void *produtor(void *arg) {
   int id = *((int*)arg);
   int i;
   for (i = 0; i < loops; i++) {
      pthread_mutex_lock(&mtx);
      while (count1 == max) {
         pthread_cond_wait(&not_full1,&mtx);
      }
      produz(i,1);
      pthread_mutex_unlock(&mtx);
      printf("Produtor %d produziu %d em 1\n", id, i);
   }
   pthread_mutex_lock(&mtx);
   produtores_restantes--;
   pthread_cond_broadcast(&not_empty1);
   pthread_mutex_unlock(&mtx);
   printf("Produtor %d finalizado\n",id);
   return NULL;
}

void *consumidor(void *arg) {
   int id = *((int*)arg);
   int tmp = 0;
   while (1) {
      pthread_mutex_lock(&mtx);
      while (count2 == 0 && (ambos_restantes > 0 || count1 > 0 || produtores_restantes > 0)) {
         pthread_cond_wait(&not_empty2,&mtx);
      }
      if (count2 == 0 && ambos_restantes == 0 && count1 == 0 && produtores_restantes == 0) {
         pthread_mutex_unlock(&mtx);
         printf("Consumidor %d finalizado\n",id);
         return NULL;
      }
      tmp = consome(2);
      pthread_mutex_unlock(&mtx);
      printf("Consumidor %d consumiu %d de 2\n", id, tmp);
   }
   return NULL;
}

void *ambos(void *arg) {
   int id = *((int*)arg);
   int tmp = 0;
   while (1) {
      pthread_mutex_lock(&mtx);
      while (count1 == 0 && produtores_restantes > 0) {
         pthread_cond_wait(&not_empty1,&mtx);
      }
      if (count1 == 0 && produtores_restantes == 0) {
         ambos_restantes--;
         pthread_cond_broadcast(&not_empty2);
         pthread_mutex_unlock(&mtx);
         printf("Ambos %d finalizado\n", id);
         return NULL;
      }
      tmp = consome(1);
      while (count2 == max) {
         pthread_cond_wait(&not_full2,&mtx);
      }
      produz(tmp,2);
      pthread_mutex_unlock(&mtx);
      printf("Ambos %d consumiu %d em 1 e produziu em 2\n", id, tmp);
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

   assert(produtores > 0 && consumidores > 0 && nambos > 0);

   produtores_restantes = produtores;
   ambos_restantes = nambos;

   buffer1 = (int *) malloc(max * sizeof(int));
   buffer2 = (int *) malloc(max * sizeof(int));
   int i;
   for (i = 0; i < max; i++) {
      buffer1[i] = 0;
      buffer2[i] = 0;
   }

   pthread_t pid[produtores], cid[consumidores],aid[nambos];
   int prod_ids[produtores], cons_ids[consumidores], ambos_ids[nambos];

   for (i = 0; i < consumidores; i++) {
      cons_ids[i] = i;
      pthread_create(&cid[i], NULL, consumidor, &cons_ids[i]);
   }
   for (i = 0; i < nambos; i++) {
      ambos_ids[i] = i;
      pthread_create(&aid[i], NULL, ambos, &ambos_ids[i]);
   }
   for (i = 0; i < produtores; i++) {
      prod_ids[i] = i;
      pthread_create(&pid[i], NULL, produtor, &prod_ids[i]);
   }
   for (i = 0; i < produtores; i++) {
      pthread_join(pid[i], NULL);
   }
   for (i = 0; i < nambos; i++) {
      pthread_join(aid[i], NULL);
   }
   for (i = 0; i < consumidores; i++) {
      pthread_join(cid[i], NULL);
   }
   return 0;
}

