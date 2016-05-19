#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "kmeans.h"
#include "dbg.h"

__attribute__((warn_unused_result, pure))
static int contained(int key, int *base, size_t size);

__attribute__((warn_unused_result, pure))
static int double_comp(const void *a, const void *b);

__attribute__((warn_unused_result, pure))
static double max(double *v, size_t size);

__attribute__((warn_unused_result, const, always_inline))
static inline double sqr_dist(double *restrict x, double *restrict y,
                              size_t offx, size_t offy, size_t dim);

typedef enum { false=0, true } bool;

extern int optv;

void lloyd(double *ex, double *c, size_t nex, size_t nat, size_t k,
           size_t *bcls, size_t *nexcl, double *rss) {

  bool updated;
  size_t i, j, new_best = 0;
  double dist_atual, dist_menor;

  size_t itr_count = 0;

  for(i = 0; i < nex; i++)
    bcls[i] = 0;

  while(1) {
    *rss = 0;
    updated = false;
    for(i = 0; i < k; i++) nexcl[i] = 0;

    //etapa 1: atribuir cada exemplo a um cluster
    for(i = 0; i < nex; i++) {
      dist_menor = INFINITY;
      for(j = 0; j < k; j++) {
        dist_atual = sqr_dist(ex, c, nat * i, nat * j, nat);
        if(dist_atual < dist_menor) {
          dist_menor = dist_atual;
          new_best = j;
        }
      }
      if(bcls[i] != new_best) {
        updated = true;
        bcls[i] = new_best;
      }
      nexcl[bcls[i]]++;
      *rss += dist_menor;
    }

    if(!updated) break;

    itr_count++;

    //etapa 2: recalcular o novo centro
    for(i = 0; i < k * nat; i++)
      c[i] = 0.0;

    for(i = 0; i < nex; i++)
      for(j = 0; j < nat; j++)
        c[j + nat * bcls[i]] += ex[j + nat * i];

    for(i = 0; i < k; i++)
      for(j = 0; j < nat; j++)
        if(nexcl[i] > 0)
          c[j + nat * i] /= nexcl[i];
  }
  printf_v1("iteracoes: %zd\n", itr_count);
}

void yinyang(double *ex, double *c, double *cant, double *ub,
             double *lb, double *var, size_t nex, size_t nat, size_t k,
             size_t *bcls, size_t *nexcl, double *rss, double lb_mult) {

  bool updated=true;
  size_t i, j, old_best, itr_count=0, calculos_evitados=0;
  double d_atual, d_menor, max_var;

  //inicializa vetor que identifica a media mais proxima para cada ponto
  for(i = 0; i < nex; i++)
    bcls[i] = 0;

  //inicializa numero de elementos de cada cluster
  for(i = 0; i < k; i++)
    nexcl[i] = 0;

  //inicializa a funcao objetivo
  *rss = 0;

  //INICIO PRIMEIRA ATRIBUICAO - LLOYD
  for(i = 0; i < nex; i++) {
    d_menor = INFINITY;

    //busca pelas medias mais proxima e segunda mais proxima
    for(j = 0; j < k; j++) {
      d_atual = sqr_dist(ex, c, nat * i, nat * j, nat);
      check(d_atual >= 0, "distancia negativa");

      //ao encontrar as distancias menor e segunda menor, inicializa ub e lb
      if(d_atual < d_menor) {
        //a menor se torna a segunda menor
        //FIXME se k=1, lb[i] == INFINITY
        //ou lb[i] == INFINITY nao e problema, pois evitara todos os calculos?
        lb[i] = d_menor;

        //atualiza d_menor e bcls
        d_menor = d_atual;
        bcls[i] = j;
      }
      else if(d_atual < lb[i]) {
        //d_atual menor que a segunda menor, atualizar segunda menor
        lb[i] = d_atual;
      }
    }

    check(d_menor != INFINITY, "d_menor == INFINITY");
    check(lb[i] != INFINITY, "lb[%zd] == INFINITY", i);
    check(bcls[i] < k, "bcls[%zd] fora do intervalo [0,k)", i);

    //d_menor definitiva do exemplo i calculada, inicializar ub
    ub[i] = d_menor;

    lb[i] *= lb_mult;

    //numero de exemplos em bcls[i] incrementado
    nexcl[bcls[i]]++;

    //funcao objetivo atualizada
    //*rss += d_menor;
  }
  //FIM PRIMEIRA ATRIBUICAO - LLOYD

  //algoritmo yinyang
  while(updated) {
    updated = false;
    //*rss = 0.0;
    itr_count++;

    //salva centro como centro anterior antes de recomputar
    for(i = 0; i < k * nat; i++) {
      cant[i] = c[i];
      c[i] = 0.0;
    }

    //recomputa cada cluster
    for(i = 0; i < nex; i++)
      for(j = 0; j < nat; j++)
        c[j + nat * bcls[i]] += ex[j + nat * i];

    for(i = 0; i < k; i++)
      for(j = 0; j < nat; j++)
        if(nexcl[i] > 0)
          c[j + nat * i] /= nexcl[i];

    //calcula variacao de cada cluster
    for(i = 0; i < k; i++)
      var[i] = sqr_dist(c, cant, nat * i, nat * i, nat);

    //atualiza limites globais de todos os exemplos
    max_var = max(var, k);
    for(i = 0; i < nex; i++) {
      ub[i] += var[bcls[i]];
      lb[i] -= max_var;
    }

    //atribui cada exemplo a um cluster
    for(i = 0; i < nex; i++) {
      if(lb[i] >= ub[i])
        calculos_evitados++;

      else {
        d_menor = INFINITY;
        old_best = bcls[i];

        //calcula a distancia do exemplo i aos j centros
        for(j = 0; j < k; j++) {
          //calcula a distancia do exemplo i ao centro j
          d_atual = sqr_dist(ex, c, nat * i, nat * j, nat);
          check(d_atual >= 0, "distancia negativa");

          if(d_atual < d_menor) {
            //atualiza d_menor e bcls
            d_menor = d_atual;
            bcls[i] = j;
          }
        }

        check(d_menor != INFINITY, "d_menor == infinito");
        check(bcls[i] < k, "bcls[%zd] fora do intervalo [0,k)", i);

        //verifica se de fato houve troca
        if(old_best != bcls[i]) {
          //check(!evitou_calculo, "calculo evitado mas exemplo mudou de grupo");
          nexcl[old_best]--;
          nexcl[bcls[i]]++;
          updated = true;
        }

        //*rss += d_menor;
      }
    }//fim atribuicao
  }//fim algoritmo

  printf_v1("iteracoes: %zd\n", itr_count);
  size_t total_calculos = itr_count * nex;
  size_t calculos_realizados = total_calculos - calculos_evitados;
  printf_v1("total de calculos: %zd\n", total_calculos);
  printf_v1("realizados: %zd\n", calculos_realizados);
  printf_v1("evitados: ");
  printf_v1("%zd\n", calculos_evitados);
  printf_v1("taxa evitados: ");
  printf("%.4f\n", (double)calculos_evitados/(double)total_calculos);

  return;

  error:
  abort();
}

void inicializa_naive(double *ex, double *c, size_t nex,
                      size_t nat, size_t k, int *gen) {
  int rnd;

  for(size_t i = 0; i < k; i++) {
    gen[i] = 0;

    do {
      //FIXME uniformidade perdida com overflow long -> int
      rnd = (int) (lrand48() % nex);
    } while(contained(rnd, gen, i) != 0);

    gen[i] = rnd;

    for(size_t j = 0; j < nat; j++)
      c[j + i * nat] = ex[j + rnd * nat];
  }
}

//TODO kmeans++
void inicializa_PP(double *ex, double *c, size_t nex,
                   size_t nat, size_t k, int *gen, double *dist) {
  size_t i, j, qtd_ja_escolhidos = 0;
  double soma, d_atual, rnd;

  //escolhe um centro inicial de maneira aleatoria uniforme
  //FIXME uniformidade perdida com overflow long -> int
  gen[0] = (int) (lrand48() % nex);
  qtd_ja_escolhidos++;

  for(i = 0; i < nat; i++)
    c[i] = ex[i + gen[0] * nat];

  //escolhe os demais centros de acordo com a distancia
  //em relacao aos ja escolhidos
  while(qtd_ja_escolhidos < k) {
    soma = 0;
    for(i = 0; i < nex; i++) {
      dist[i] = INFINITY;
      for(j = 0; j < qtd_ja_escolhidos; j++) {
        d_atual = sqr_dist(ex, c, nat * i, nat * j, nat);

        if(d_atual < dist[i])
          dist[i] = d_atual;
      }
      soma += dist[i];
    }
    dist[i] /= soma;

    qsort(dist, nex, sizeof(double), double_comp);

    i = 0;
    rnd = drand48();
    while(rnd < dist[i]) {
      i++;
    }

    //validar o centro escolhido, verificando se ele ja foi escolhido em outra
    //iteracao. se o centro nao for validado qtd_ja_escolhidos nao sera
    //incrementada e o loop sera realizado mais uma vez *HACK*
    //FIXME gerar outro numero aleatorio ao inves de recalcular distancias
    if(!contained((int) i, gen, qtd_ja_escolhidos))
      gen[qtd_ja_escolhidos++] = (int)i;
  }
}

//helper functions

int contained(int key, int *base, size_t size) {
  for(size_t i = 0; i < size; i++) {
    if(key == base[i])
      return 1;
  }
  return 0;
}

int double_comp(const void *a, const void *b) {
  if((*(double *) a) > (*(double *) b)) return 1;
  if((*(double *) a) < (*(double *) b)) return -1;
  return 0;
}

double max(double *v, size_t size) {
  double maior = v[0];

  for(size_t i = 1; i < size; i++)
    if(v[i] > maior)
      maior = v[i];

  return maior;
}

double sqr_dist(double *restrict x, double *restrict y,
                size_t offx, size_t offy, size_t dim) {
  double d = 0.0, dif;

  for(size_t i = 0; i < dim; i++) {
    dif = x[offx + i] - y[offy + i];
    d += dif*dif;
  }

  return sqrt(d);
}
