#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

int pertence(long x, long *v, int tam);
float max(float *v, int size);
float delta(float xfin, float xini);
float distancia(float *v1, float *v2, int n);
int olhaChar();
time_t seed();
long aleatorio(unsigned max);
long *gera(unsigned qtdGrupos, unsigned max);
void InicializaCentros
    (float ***Centros, float ***Exemplos, long *index,
     unsigned K, unsigned nColunas, FILE *Gerados, char *sep);
//void liberaDim2(void **ptr, int n);
