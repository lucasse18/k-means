#include "utils.h"

inline double max(double *v, unsigned int size) {
  double maior = v[0];
  for(unsigned i = 1; i < size; i++) {
    if(v[i] > maior)
      maior = v[i];
  }
  return maior;
}

inline double distancia(double *v1, double *v2, size_t size) {
  double delt, dist = 0;
  for(unsigned i = 0; i < size; i++) {
    delt = v2[i] - v1[i];
    dist += delt*delt;
  }
  return dist;
}

inline int olhaChar(FILE *entrada) {
  int c;
  c = fgetc(entrada);
  if(c != EOF)
    ungetc(c, entrada);
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

int comp(const void *a, const void *b) {
  if (*(long double *)a < (*(long double *)b)) return -1;
  if (*(long double *)a > (*(long double *)b)) return 1;
  return 0;
}

inline double menorDistancia(double *Ponto, double **Centros, unsigned nAtributos, unsigned size) {
  double distanciaAtual;
  double menorDistancia = distancia(Ponto, Centros[0], size);
  for(unsigned i = 1; i < size; i++) {
    distanciaAtual = distancia(Ponto, Centros[i], nAtributos);
    if(distanciaAtual < menorDistancia)
      menorDistancia = distanciaAtual;
  }
  return menorDistancia;
}

int nao_unico(unsigned *v, unsigned n, unsigned size) {
  for (unsigned i = 0; i < size; ++i) {
    if (v[i] == n)
      return 1;
  }
  return 0;
}

void InicializaCentrosClassico(double **Centros, double **Exemplos, Data data, FILE *Gerados, char *separador) {

  long rnd = 0;
  unsigned *index = calloc(data.K, sizeof(*index));
  for(unsigned i = 0; i < data.K; i++) {
    while(nao_unico(index, rnd, data.K))
      rnd = lrand48()%data.nExemplos;

    index[i] = rnd;

    for(unsigned j = 0; j < data.nAtributos; j++)
      Centros[i][j] = Exemplos[index[i]][j];
  }

  for(unsigned i = 0; i < data.K; i++) {
    for(unsigned j = 0; j < data.nAtributos; j++)
      fprintf(Gerados, "%f%s", Centros[i][j], separador);

    fprintf(Gerados, " Exemplo %u\n", index[i]);
  }

  free(index);
}


void InicializaCentrosPP(double **Centros, double **Exemplos, Data data, FILE *Gerados, char *separador) {

  double rand;
  long double *distancias = calloc(data.nExemplos, sizeof(*distancias));
  unsigned *index = malloc(data.K * sizeof(*index));

  //inicializa primeiro centro aleatorio
  index[0] = aleatorio(data.nExemplos);
  for(unsigned i = 0; i < data.nAtributos; i++)
    Centros[0][i] = Exemplos[index[0]][i];

  //inicializa os demais centros proporcionalmente as distancias dos centros ja escolhidos
  for(unsigned i = 1; i < data.K; i++) {
    //calcula e armazena a menor distancia de cada ponto aos centros ja escolhidos
    for(unsigned j = 0; j < data.nExemplos; j++)
      distancias[j] = menorDistancia(Exemplos[j], Centros, data.nAtributos, i);

    //encontra index do exemplo que sera escolhido como proximo centro
    qsort(distancias, data.nExemplos, sizeof(*distancias), comp);
    for(unsigned j = 0; j < data.nExemplos; j++)
      distancias[j] = distancias[j]/distancias[data.nExemplos - 1];

    do {
      index[i] = 0;
      rand = drand48();
      while(distancias[index[i]] < rand)
        (index[i])++;
    } while(nao_unico(index, index[i], i));

    //faz a atribuicao
    for(unsigned j = 0; j < data.nAtributos; j++)
      Centros[i][j] = Exemplos[index[i]][j];
  }

  for(unsigned i = 0; i < data.K; i++) {
    for(unsigned j = 0; j < data.nAtributos; j++)
      fprintf(Gerados, "%.4f%s", Centros[i][j], separador);

    fprintf(Gerados, " Exemplo %u\n", index[i]);
  }

  free(distancias);
  free(index);
}
