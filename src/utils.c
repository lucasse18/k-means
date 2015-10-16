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

//FIXME em algumas ocasioes retorna NaN
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

inline unsigned aleatorio(unsigned max) {
  return (unsigned)lrand48()%max;//FIXME cast gera overflow?
}

int comparaInt(const void *a, const void *b) {
  if (*(int *)a < (*(int *)b)) return -1;
  if (*(int *)a > (*(int *)b)) return 1;
  return 0;//igual
}

float menorDistancia(float *Ponto, float **Centros, unsigned nAtributos, unsigned size) {
  float distanciaAtual;
  float menorDistancia = distancia(Ponto, Centros[0], size);
  for(unsigned i = 1; i < size; i++) {
    distanciaAtual = distancia(Ponto, Centros[i], nAtributos);
    if(distanciaAtual < menorDistancia)
      menorDistancia = distanciaAtual;
  }
  return menorDistancia;
}

void InicializaCentros
    (float ***Centros, float **Exemplos,
     Data data, FILE *Gerados, char *separador) {

  double rand;
  float *distancias = malloc(data.nExemplos *sizeof(distancias));
  unsigned *index = calloc(data.K, sizeof(*index));

  //inicializa primeiro centro aleatorio
  index[0] = aleatorio(data.K);
  for(unsigned i = 0; i < data.nAtributos; i++) {
    (*Centros)[0][i] = Exemplos[index[0]][i];
  }

  //inicializa os demais centros proporcionalmente as distancias dos centros ja escolhidos
  for(unsigned i = 1; i < data.K; i++) {
    //armazena a menor distancia de cada ponto aos centros ja escolhidos
    for(unsigned j = 0; j < data.nExemplos; j++)
      distancias[j] = menorDistancia(Exemplos[j], (*Centros), data.nAtributos, i);

    //encontra index do exemplo que sera escolhido como proximo centro
    for(unsigned j = 0; j < data.nExemplos; j++)
      distancias[j] = distancias[j]/distancias[data.nExemplos - 1];
    qsort(distancias, data.nExemplos, sizeof(*distancias), comparaInt);
    rand = drand48();
    while(distancias[index[i]] < rand)
      (index[i])++;//alocado com calloc, ja zerado

    //faz a atribuicao
    for(unsigned j = 0; j < data.nAtributos; j++) {
      (*Centros)[i][j] = Exemplos[index[i]][j];
    }
  }

  free(distancias);
  free(index);

  for(unsigned i = 0; i < data.K; i++) {
    for(unsigned j = 0; j < data.nAtributos; j++) {
      fprintf(Gerados, "%f", (*Centros)[i][j]);
      fprintf(Gerados, "%s", separador);
    }
    fprintf(Gerados, " Exemplo %d\n", index[i]);
  }
}
