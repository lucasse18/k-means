#include "utils.h"

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
  //fprintf(stderr,"WARNING seed: %d\n", 1067700750);
  //return 1067700750;
  return tempo.tv_sec*tempo.tv_nsec;
}

inline long aleatorio(unsigned max) {
  return lrand48()%max;
}

int comparaFloat(const void *a, const void *b) {
  if (*(float *)a < (*(float *)b)) return -1;
  if (*(float *)a > (*(float *)b)) return 1;
  return 0;//igual
}

inline float menorDistancia(float *Ponto, float **Centros, unsigned nAtributos, unsigned size) {
  float distanciaAtual;
  float menorDistancia = distancia(Ponto, Centros[0], size);
  for(int i = 1; i < size; i++) {
    distanciaAtual = distancia(Ponto, Centros[i], nAtributos);
    if(distanciaAtual < menorDistancia)
      menorDistancia = distanciaAtual;
  }
  return menorDistancia;
}

int nao_unico(long *v, int size, long n) {
  for (int i = 0; i < size; ++i) {
    if (v[i] == n)
      return 1;
  }
  return 0;
}

void InicializaCentros(float ***Centros, float **Exemplos, Data data, FILE *Gerados, char *separador) {

  double rand;
  float *distancias = malloc(data.nExemplos *sizeof(distancias));
  long *index = malloc(data.K * sizeof(*index));

  //inicializa primeiro centro aleatorio
  index[0] = aleatorio(data.nExemplos);
  for(int i = 0; i < data.nAtributos; i++)
    (*Centros)[0][i] = Exemplos[index[0]][i];

  //inicializa os demais centros proporcionalmente as distancias dos centros ja escolhidos
  for(int i = 1; i < data.K; i++) {
    //calcula e armazena a menor distancia de cada ponto aos centros ja escolhidos
    for(int j = 0; j < data.nExemplos; j++)
      distancias[j] = menorDistancia(Exemplos[j], (*Centros), data.nAtributos, i);

    //encontra index do exemplo que sera escolhido como proximo centro
    qsort(distancias, data.nExemplos, sizeof(*distancias), comparaFloat);
    for(int j = 0; j < data.nExemplos; j++)
      distancias[j] = distancias[j]/distancias[data.nExemplos - 1];

    do {
      rand = drand48();
      index[i] = 0;
      while(distancias[index[i]] < rand)
        (index[i])++;
    } while(nao_unico(index, i, index[i]));

    //faz a atribuicao
    for(int j = 0; j < data.nAtributos; j++)
      (*Centros)[i][j] = Exemplos[index[i]][j];
  }

  for(int i = 0; i < data.K; i++) {
    for(int j = 0; j < data.nAtributos; j++) {
      fprintf(Gerados, "%.4f", (*Centros)[i][j]);
      fprintf(Gerados, "%s", separador);
    }
    fprintf(Gerados, " Exemplo %ld\n", index[i]);
  }

  free(distancias);
  free(index);
}
