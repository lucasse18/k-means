#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "kmeans.h"

void lloyd(double *ex, double *c, int nex, int nat, int k, int *bcls,
           int *nexcl, double *rss) {

  int trocou;
  int i, j, l, novo_melhor = -1;
  double dist_atual, dist_menor, delta;

  //inicializa todos os exemplos no cluster -1
  for(i = 0; i < nex; i++)
    bcls[i] = -1;

  int flag = 1;

  while(1) {

    *rss = 0;
    trocou = 0;
    for(i = 0; i < k; i++) nexcl[i] = 0;

    //etapa 1: atribuir cada exemplo a um cluster
    for(i = 0; i < nex; i++) {
      dist_menor = INFINITY;
      for(j = 0; j < k; j++) {
        dist_atual = 0.0;
        for(l = 0; l < nat; l++) {
          delta = ex[l + nat * i] - c[l + nat * j];
          dist_atual += delta * delta;
        }
        if(dist_atual < dist_menor) {
          dist_menor = dist_atual;
          novo_melhor = j;
        }
      }
      if(bcls[i] != novo_melhor) {
        trocou = 1;
        bcls[i] = novo_melhor;
      }
      nexcl[bcls[i]]++;
      *rss += dist_menor;
    }

    if(flag) {
      printf("\nPRIMEIRA ATRIBUICAO:\n");
      printf("RSS: %.3f\n", *rss);
      for(i = 0; i < k; i++)
        printf("Numero de exemplos no cluster[%d]: %d\n", i, nexcl[i]);
      printf("\n");
      flag = 0;
    }

    printf("RSS: %.3f\n", *rss);

    if(!trocou) break;

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
}

double max(double *v, int size) {

  if(size == 1)
    return v[0];

  double maior = -INFINITY;

  for(int i = 0; i < size; i++)
    if(v[i] > maior)
      maior = v[i];

  if(maior == -INFINITY)
    abort();

  return maior;
}

void yinyang(double *ex, double *c, double *cant, double *ub,
             double *lb, double *var, int nex, int nat, int k,
             int *bcls, int *secbcls, int *nexcl, double *rss) {

  int trocou = 1, itr_count=0;
  int i, j, l, antigo_melhor;
  double d_atual, d_menor, seg_d_menor, delta, maxvar;

  //inicializa vetores que identificam as medias mais proximas para cada ponto
  for(i = 0; i < nex; i++)
    bcls[i] = secbcls[i] = -1;

  //inicializa numero de elementos de cada cluster
  for(i = 0; i < k; i++)
    nexcl[i] = 0;

  //inicializa a funcao objetivo
  *rss = 0;

  //INICIO PRIMEIRA ATRIBUICAO
  for(i = 0; i < nex; i++) {

    d_menor = seg_d_menor = INFINITY;

    //busca pelas medias mais proxima e segunda mais proxima
    for(j = 0; j < k; j++) {
      d_atual = 0.0;
      for(l = 0; l < nat; l++) {
        delta = ex[l + nat * i] - c[l + nat * j];
        d_atual += delta * delta;
      }

      //ao encontrar as distancias menor e segunda menor, inicializa ub e lb
      if(d_atual < d_menor) {
        //a menor se torna a segunda menor
        seg_d_menor = lb[i] = d_menor;
        secbcls[i] = bcls[i];

        d_menor = ub[i] = d_atual;
        bcls[i] = j;
      }
      else if(d_atual < seg_d_menor) {
        seg_d_menor = lb[i] = d_atual;
        secbcls[i] = j;
      }
    }

    nexcl[bcls[i]]++;
    *rss += d_menor;
  }
  //FIM PRIMEIRA ATRIBUICAO

  printf("\nPRIMEIRA ATRIBUICAO:\n");
  printf("RSS: %.3f\n", *rss);
  for(i = 0; i < k; i++)
    printf("Numero de exemplos no cluster[%d]: %d\n", i, nexcl[i]);
  printf("\n");

  //salva centro como centro anterior antes de recomputar
  /*
  for(i = 0; i < k * nat; i++) {
    cant[i] = c[i];
    c[i] = 0.0;
  }
  */

  //recomputa cada centro
  for(i = 0; i < nex; i++)
    for(j = 0; j < nat; j++)
      c[j + nat * bcls[i]] += ex[j + nat * i];

  for(i = 0; i < k; i++)
    for(j = 0; j < nat; j++)
      if(nexcl[i] > 0)
        c[j + nat * i] /= nexcl[i];

  /*
  //calcula a variacao de cada cluster
  for(i = 0; i < k; i++) {
    var[i] = 0.0;
    for(j = 0; j < nat; j++) {
      delta = cant[j + nat * i] - c[j + nat * i];
      var[i] += delta * delta;
    }
  }

  //atualiza limites globais de todos os exemplos
  maxvar = max(var, k);
  assert(maxvar != INFINITY);
  for(i = 0; i < nex; i++) {
    ub[i] += var[bcls[i]];
    lb[i] -= maxvar;
  }
  */

  int calculos_evitados=0;
  while(trocou) {
    itr_count++;
    *rss = 0.0;
    trocou = 0;

    //etapa 1: atribuir cada exemplo a um cluster
    for(i = 0; i < nex; i++) {

      if(lb[i] > ub[i]) {
        d_menor = seg_d_menor = INFINITY;
        antigo_melhor = bcls[i];

        //calcula a distancia do exemplo i aos j centros
        for(j = 0; j < k; j++) {
          //calcula a distancia do exemplo i ao centro j
          d_atual = 0.0;
          for(l = 0; l < nat; l++) {
            delta = ex[l + nat * i] - c[l + nat * j];
            d_atual += delta * delta;
          }

          if(d_atual < d_menor) {
            //a menor se torna a segunda menor
            seg_d_menor = d_menor;

            //melhor cluster se torna o segundo melhor cluster
            //na primeira troca o conteudo de secbcls podera nao fazer sentido,
            //mas sera atualizado na proxima iteracao ja que seg_d_menor
            //ainda sera INFINITY (antigo valor de d_menor)
            secbcls[i] = bcls[i];

            //atualiza d_menor e melhor cluster
            d_menor = d_atual;
            bcls[i] = j;
          }
          else if(d_atual < seg_d_menor) {
            seg_d_menor = d_atual;
            secbcls[i] = j;
          }
        }

        //verifica se de fato houve troca
        if(antigo_melhor != bcls[i]) {
          nexcl[antigo_melhor]--;
          nexcl[bcls[i]]++;
          trocou = 1;
        }

        *rss += d_menor;
      }
      else {
        //FIXME como calcular rss?
        calculos_evitados++;
        *rss += ub[i];
      }
    }
    printf("RSS: %.3f\n", *rss);

    if(!trocou) break;

    //etapa 2: recalcular os novos centros

    //salva centro como centro anterior antes de recomputar
    for(i = 0; i < k * nat; i++) {
      cant[i] = c[i];
      c[i] = 0.0;
    }

    //recomputa cada centro
    for(i = 0; i < nex; i++)
      for(j = 0; j < nat; j++)
        c[j + nat * bcls[i]] += ex[j + nat * i];

    for(i = 0; i < k; i++)
      for(j = 0; j < nat; j++)
        if(nexcl[i] > 0)
          c[j + nat * i] /= nexcl[i];

    //calcula variacao de cada cluster
    for(i = 0; i < k; i++) {
      var[i] = 0.0;
      for(j = 0; j < nat; j++) {
        delta = cant[j + nat * i] - c[j + nat * i];
        var[i] += delta * delta;
      }
    }

    //atualiza limites globais de todos os exemplos
    maxvar = max(var, k);
    for(i = 0; i < nex; i++) {
      ub[i] += var[bcls[i]];
      lb[i] -= maxvar;
    }

  }// fim while

  int total_operacoes = itr_count * nex;
  printf("\nIteracoes: %d\n", itr_count);
  printf("Total de Calculos: %d\n", total_operacoes);
  printf("Realizados: %d\n", total_operacoes - calculos_evitados);
  printf("Evitados: %d\n", calculos_evitados);
  printf("Taxa: %.2f\n\n", (double)calculos_evitados/(double)total_operacoes);
}

int buscaLinear(int key, int *base, int size) {
  int i;
  for(i = 0; i < size; i++) {
    if(key == base[i])
      return 1;
  }
  return 0;
}

void inicializa_naive(double *ex, double *c, int nex,
                      int nat, int k, int *gen) {
  int i, j,rnd;

  for(i = 0; i < k; i++) {
    gen[i] = 0;

    do {
      rnd = (int) (lrand48() % nex);//FIXME long to int overflow
    } while(buscaLinear(rnd, gen, i) != 0);

    gen[i] = rnd;

    for(j = 0; j < nat; j++)
      c[j + i * nat] = ex[j + rnd * nat];
  }
}

int compara_double(const void *a, const void *b) {
  if((*(double *) a) > (*(double *) b)) return 1;
  if((*(double *) a) < (*(double *) b)) return -1;
  return 0;
}

void inicializa_PP(double *ex, double *c, int nex,
                   int nat, int k, int *gen, double *dist) {
  int i, j, l, qtd_ja_escolhidos = 0;
  double soma, delta, dist_atual, rnd;

  //escolhe um centro inicial de maneira aleatoria uniforme
  gen[0] = (int) (lrand48() % nex);//FIXME long to int overflow
  qtd_ja_escolhidos++;
  for(i = 0; i < nat; i++)
      c[i] = ex[i + gen[0] * nat];

  //escolhe os demais centros de acordo com a distancia
  //em relacao aos ja escolhidos
  while(qtd_ja_escolhidos < k) {
    soma = 0;
    for(i = 0; i < nex; i++) {
      dist[i] = INFINITY;
      for(l = 0; l < qtd_ja_escolhidos; l++) {
        dist_atual = 0;
        for(j = 0; j < nat; j++) {
          delta = ex[l + nat * i] - c[l + nat * j];;
          dist_atual += delta * delta;
        }

        if(dist_atual < dist[i])
          dist[i] = dist_atual;

      }
      soma += dist[i];
    }
    dist[i] /= soma;
    qsort(dist, (size_t)nex, sizeof(double), compara_double);

    i = 0;
    rnd = drand48();
    while(rnd < dist[i]) {
      i++;
    }

    //validar o centro escolhido, verificando se ele ja foi escolhido em outra
    //iteracao. se o centro nao for validado qtd_ja_escolhidos nao sera
    //incrementada e o loop sera realizado mais uma vez HACK
    //FIXME gerar outro numero aleatorio ao inves de recalcular distancias
    if(!buscaLinear(i, gen, qtd_ja_escolhidos))
      gen[qtd_ja_escolhidos++] = i;

  }
}
