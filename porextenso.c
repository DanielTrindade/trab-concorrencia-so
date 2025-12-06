#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>

#define LOOPS 1000
#define NGERADORES 5

static const char * unidades[]  = { "", "Um", "Dois", "Tres", "Quatro", "Cinco", "Seis", "Sete", "Oito", "Nove" };
static const char * dezVinte[] = { "", "Onze", "Doze", "Treze", "Quatorze", "Quinze", "Dezesseis", "Dezessete", "Dezoito", "Dezenove" };
static const char * dezenas[]   = { "", "Dez", "Vinte", "Trinta", "Quarenta", "Cinquenta", "Sessenta", "Setenta", "Oitenta", "Noventa" };
static const char * centenas[]  = { "", "Cento", "Duzentos", "Trezentos", "Quatrocentos", "Quinhentos", "Seiscentos", "Setecentos", "Oitocentos", "Novecentos" };

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_pedido = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_processado = PTHREAD_COND_INITIALIZER;

static char nome[256];
static int pedido_disponivel = 0;
static int encerrando = 0;
static int geradores_ativos = NGERADORES;
static int valor_pendente = 0;

char * strcatb( char * dst, const char * src )
{
   size_t len = strlen(src);
   memmove( dst + len, dst, strlen(dst) + 1 );
   memcpy( dst, src, len );
   return dst;
}

static void converte_para_extenso(int numero, char *dest)
{
   char *e = " e ";
   int c,d,dv,u;

   c=numero/100;
   d=numero/10-c*10;
   u=numero-(numero/10)*10;
   dv=d*10+u;
   dest[0]='\0';

   if (numero == 0){
      strcatb(dest,"Zero");
      return;
   }
   if (numero<10){
      strcatb(dest,unidades[u]);
      return;
   }
   if ((dv>10) && (dv<20)){
      strcatb(dest,dezVinte[dv-10]);
   }
   else
   {
      if (u>0){
         strcatb(dest,unidades[u]);
      }
      if (d>0){
         if(u>0){
            strcatb(dest,e);
         }
         strcatb(dest,dezenas[d]);
      }
   }
   if (numero<100){
      return;
   }

   if ((d==0)&&(u==0))
   {
      if (c==1)
         strcatb(dest,"Cem");
      else
         strcatb(dest,centenas[c]);
   }
   else
   {
      if(dest[0]!='\0'){
         strcatb(dest,e);
      }
      strcatb(dest,centenas[c]);
   }
}

// Thread que aguarda pedidos de escrita por extenso e processa um por vez
void* porExtenso(void*arg){
   (void)arg;
   while(1){
      int numero_local;
      char nome_local[256];

      pthread_mutex_lock(&mtx);
      while(!pedido_disponivel && !encerrando){
         pthread_cond_wait(&cond_pedido,&mtx);
      }
      if(!pedido_disponivel && encerrando){
         pthread_mutex_unlock(&mtx);
         break;
      }
      numero_local = valor_pendente;
      pthread_mutex_unlock(&mtx);

      converte_para_extenso(numero_local,nome_local);

      pthread_mutex_lock(&mtx);
      strcpy(nome,nome_local);
      pedido_disponivel = 0;
      pthread_cond_broadcast(&cond_processado);
      pthread_mutex_unlock(&mtx);
   }
   return NULL;
}

// Thread geradora que envia numeros para a thread porExtenso
void *geraNumeros(void* arg){
   int id = *((int*)arg),i,nold;
   for(i=0;i<LOOPS;i++){
      nold = rand()%1000;
      pthread_mutex_lock(&mtx);
      while(pedido_disponivel){
         pthread_cond_wait(&cond_processado,&mtx);
      }
      valor_pendente = nold;
      pedido_disponivel = 1;
      pthread_cond_signal(&cond_pedido);
      while(pedido_disponivel){
         pthread_cond_wait(&cond_processado,&mtx);
      }
      char nome_local[256];
      strcpy(nome_local,nome);
      pthread_mutex_unlock(&mtx);
      printf("Thread:%d I:%d Numero:%d:%s\n",id,i,nold,nome_local);
   }

   pthread_mutex_lock(&mtx);
   geradores_ativos--;
   if(geradores_ativos==0){
      encerrando = 1;
      pthread_cond_broadcast(&cond_pedido);
   }
   pthread_mutex_unlock(&mtx);
   return NULL;
}

int main(){
   int i;
   pthread_t p[NGERADORES],ext;
   int ids[NGERADORES];

   srand((unsigned)time(NULL));

   pthread_create(&ext,NULL,porExtenso,NULL);
   for(i=0;i<NGERADORES;i++){
      ids[i]=i;
      pthread_create(&p[i],NULL,geraNumeros,(void*)&ids[i]);
   }

   for(i=0;i<NGERADORES;i++){
      pthread_join(p[i],NULL);
   }
   pthread_join(ext,NULL);

   return 0;
}
