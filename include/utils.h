#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

typedef struct data {
  unsigned nExemplos;
  unsigned nAtributos;
  unsigned K;
}Data;

float max(float *v, int size);
float delta(float xfin, float xini);
float distancia(float *v1, float *v2, int n);
int olhaChar();

time_t seed();
unsigned aleatorio(unsigned max);

int comparaInt(const void *a, const void *b);
float menorDistancia(float *Ponto, float **Centros, unsigned nAtributos, unsigned size);
void InicializaCentros
    (float ***Centros, float **Exemplos,
     Data data, FILE *Gerados, char *separador);
