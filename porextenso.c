#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

char nome[256];
volatile int n;
#define LOOPS 1000
#define NTHREADS 10

static const char * unidades[]  = { "", "Um", "Dois", "Tres", "Quatro", "Cinco", "Seis", "Sete", "Oito", "Nove" };
static const char * dezVinte[] = { "", "Onze", "Doze", "Treze", "Quatorze", "Quinze", "Dezesseis", "Dezessete", "Dezoito", "Dezenove" };
static const char * dezenas[]   = { "", "Dez", "Vinte", "Trinta", "Quarenta", "Cinquenta", "Sessenta", "Setenta", "Oitenta", "Noventa" };
static const char * centenas[]  = { "", "Cento", "Duzentos", "Trezentos", "Quatrocentos", "Quinhentos", "Seiscentos", "Setecentos", "Oitocentos", "Novecentos" };

char * strcatb( char * dst, const char * src )
{
   size_t len = strlen(src);
   memmove( dst + len, dst, strlen(dst) + 1 );
   memcpy( dst, src, len );
   return dst;
}


// Modifique esta funcao para rodar constantemente, esperando por alguma thread geraNumeros avise que tem um novo numero para ser escrito por extenso.
// Apos a escrita na variavel nome, a thread deve voltar a esperar por uma nova requisicao
void* porExtenso(void*arg){
   char *e = " e ";
   int c,d,dv,u; 

   //   while(1){
   c=n/100; 
   d=n/10-c*10;
   u=n-(n/10)*10;
   dv=d*10+u;
   nome[0]='\0';
   if (n<10){
      if(n==0){
         strcatb(nome,"zero");
         strcatb(nome,unidades[u]);
      }
      return nome;
   }
   // Onze a dezenove  
   if ((dv>10) && (dv<20))
      strcatb(nome,dezVinte[dv-10]);
   else
   {    
      strcatb(nome,unidades[u]);
      if (u==0)
         strcatb(nome,dezenas[d]);
      else
      {
         if(d>0){
            strcatb(nome,e);
            strcatb(nome,dezenas[d]);
         }
      }
   }
   if (n<100)
      return nome;
   // Inteiro
   if ((d==0)&&(u==0))
   {
      if (c==1)
         strcatb(nome,"cem");
      else
         strcatb(nome,centenas[c]);
   }
   else
   {
      strcatb(nome,e);
      strcatb(nome,centenas[c]);
   }

   //}
}

void *geraNumeros(void* arg){
   int id = *((int*)arg),i,nold;
   for(i=0;i<LOOPS;i++){
      nold = rand()%1000;
      n= nold;
      porExtenso(NULL);
      // Avisa a thread porExtenso que pode escrever o numero por extenso
      printf("Thread:%d I:%d Numero:%d:%s\n",id,i,nold,nome);
   }
}


int main(){
   int i;
   pthread_t p[NTHREADS],ext;

   /*Exemplo de funcionamento de porExtenso*/
   n=101;
   porExtenso(NULL);
   printf("%s\n",nome);
   /************* Geracao de threads ****************/

   pthread_create(&ext,NULL,porExtenso,NULL);
   for(i=0;i<NTHREADS;i++){
      pthread_create(&p[i],NULL,geraNumeros,(void*)&i);
   }

   for(i=0;i<NTHREADS;i++){
      pthread_join(p[i],NULL);
   }

}
