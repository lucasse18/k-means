#include "utils.h"

inline int pertence(long x, long *v, int tam) {
  for(int i = 0; i < tam; i++) {
    if(x == v[i])
      return 1;
  }
  return 0;
}

inline float max(float *v, int size) {
  float maior = v[0];
  for(int i = 1; i < size; i++) {
    if(v[i] > maior)
      maior = v[i];
  }
  return maior;
}

inline float delta(float xfin, float xini) {
  return xfin - xini;
}

inline float distancia(float *v1, float *v2, int n) {
  float d = 0;
  for(int i = 0; i < n; i++)
    d += pow(delta(v2[i], v1[i]), 2.0);
  return d;
}

inline int olhaChar() {
  int c;
  c = fgetc(stdin);
  if(c != EOF)
    ungetc(c, stdin);
  return c;
}

inline time_t seed() {
  struct timespec tempo;
  if (clock_gettime(CLOCK_REALTIME, &tempo) == -1) {
    perror("clock_gettime");
    exit(EXIT_FAILURE);
  }
  return tempo.tv_sec*tempo.tv_nsec;
}

inline long aleatorio(unsigned max) {
  return lrand48()%max;
}

inline long *gera(unsigned qtdGrupos, unsigned max) {
  long index;
  long *gerados = calloc(qtdGrupos, sizeof(long));

  srand48((unsigned int) seed());

  for(unsigned i = 0; i < qtdGrupos; i++) {
    index = aleatorio(max);
    if(pertence(index, gerados, i))
      i--;
    else
      gerados[i] = index;
  }
  return gerados;
}

//FIXME menos parametros
void InicializaCentros
    (float ***Centros, float ***Exemplos, long *index,
     unsigned K, unsigned nColunas, FILE *Gerados, char *sep) {

  //inicializa centros aleatorios
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++)
      (*Centros)[i][j] = (*Exemplos)[index[i]][j];
  }
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++) {
      fprintf(Gerados, "%f", (*Centros)[i][j]);
      fprintf(Gerados, "%s", sep);
    }
    fprintf(Gerados, " Exemplo %ld\n", index[i]);
  }
}
