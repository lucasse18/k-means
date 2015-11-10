#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef struct data {
  unsigned nExemplos;
  unsigned nAtributos;
  unsigned K;
}Data;

int olhaChar(FILE *entrada);

double distancia(double *v1, double *v2, size_t size);
double menorDistancia(double *Ponto, double **Centros, unsigned nAtributos, unsigned size);
double max(double *v, unsigned int size);

time_t seed();
long aleatorio(unsigned max);
int comparaFloat(const void *a, const void *b);
void InicializaCentrosClassico(double **Centros, double **Exemplos, Data data, FILE *Gerados, char *separador);
void InicializaCentrosPP(double **Centros, double **Exemplos, Data data, FILE *Gerados, char *separador);
