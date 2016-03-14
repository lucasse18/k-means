#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include "../include/dataset.h"
#include "../include/kmeans.h"

void die(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

int main(int argc, char *argv[]) {

  FILE *data_file = NULL;
  FILE *outfile = stdout;
  static int k = 0;
  static int class_flag = 0;
  int algorithm = 1;//default lloyd

  static struct option long_opts[] = {
    {"dataset",   required_argument, 0,         'd'},//FIXME opcao obrigatoria
    {"outfile",   required_argument, 0,         'o'},//default stdout, ainda nao usado
    {"algorithm", required_argument, 0,         'a'},//default lloyd(1)
    {"groups",    required_argument, &k,        'k'},//default criterio de oliveira
    {"no-class",  no_argument,       &class_flag, 1},//TODO ainda nao usado
    {0,           0,                 0,           0}
  };

  int opt_index = 0;
  int c;

  while((c = getopt_long (argc, argv, "d:o:a:k:n", long_opts, &opt_index)) != -1) {

    switch(c) {
    case 0:
      break;

    //TODO help message

    case 'd':
      //printf("option -d with value `%s'\n", optarg);
      if(data_file != NULL)//caso passar -d duas vezes
        fclose(data_file);
      if(!strcmp(optarg, "stdin")  ||
         !strcmp(optarg, "stdout") ||
         !strcmp(optarg, "NULL")) {
        die("please specify a valid archive name");
      }

      data_file = fopen(optarg, "r");
      break;

    case 'o':
      //printf("option -o with value `%s'\n", optarg);
      if(outfile != stdout)//caso passar -o duas vezes
        fclose(outfile);
      if(!strcmp(optarg, "stdin")  ||
         !strcmp(optarg, "stdout") ||
         !strcmp(optarg, "NULL")) {
        die("please specify a valid archive name");
      }

      outfile = fopen(optarg, "w");
      break;

    case 'a':
      //printf("option -a with value `%s'\n", optarg);
      if(strcmp("lloyd", optarg) == 0)
        algorithm = 1;
      else if(strcmp("yinyang", optarg) == 0)
        algorithm = 2;
      else if(strcmp("kmeanspp", optarg) == 0)
        algorithm = 3;
      else if(strcmp("yinyangpp", optarg) == 0)
        algorithm = 4;
      else
        fprintf(stderr, "warning: invalid algorithm, defaulting to lloyd.\n");
      break;

    case 'k':
      //printf("option -k with value `%s'\n", optarg);
      k = atoi(optarg);
      if(k <= 0) {
        die("the number of groups (option -k) must be greater than zero");
      }
      break;

    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
      break;

    default:
      abort();
    }
  }

  if(data_file == NULL) {
    die("please specify at least the dataset to be clustered");
  }

  Dataset data;
  DATASET_INIT(data, data_file);

  //se k nao foi setado, utilizar o criterio de oliveira
  if(k == 0) {
    if(data.nex <= 100)
      k = sqrt(data.nex);
    else
      k = 5 * log10(data.nex);
  }

  printf("NEX: %d, ", data.nex);
  printf("NAT: %d, ", data.nat);
  printf("K: %d\n", k);

  double *centros = malloc(k * data.nat * sizeof(double));
  assert(centros);
  int *bcls = malloc(data.nex * sizeof(int));
  assert(bcls);
  int *nexcl = malloc(k * sizeof(int));
  assert(nexcl);
  int gerados[k];
  double rss = 0.0;

  //yin yang
  double *cant = 0;
  double *ub = 0;
  double *lb = 0;
  double *var = 0;
  int *secbcls = 0;

  //kmeanspp
  double *dist;

  srand48(1);

  switch(algorithm) {
  case 1:
    inicializa_naive(data.ex.vec, centros, data.nex, data.nat, k, gerados);
    lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);
    break;

  case 2:
    cant = malloc(k * data.nat * sizeof(double));
    assert(cant);
    ub = malloc(data.nex * sizeof(double));
    assert(ub);
    lb = malloc(data.nex * sizeof(double));
    assert(lb);
    var = malloc(k * sizeof(double));
    assert(var);
    secbcls = malloc(data.nex * sizeof(int));
    assert(secbcls);

    yinyang(data.ex.vec, centros, cant, ub, lb, var, data.nex,
            data.nat, k, bcls, secbcls, nexcl, &rss);

    free(cant);
    free(ub);
    free(lb);
    free(var);
    free(secbcls);
    break;

  case 3:
    dist = malloc(data.nex * sizeof(double));
    assert(dist);

    inicializa_PP(data.ex.vec, centros, data.nex, data.nat, k, gerados, dist);
    lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);

    free(dist);
    break;

  default:
    //nao faz sentido cair em default, abortar
    abort();
  }

  for(int i = 0; i < k; i++)
    printf("Exemplos no cluster [%d]: %d\n", i, nexcl[i]);

  DATASET_FREE(data);
  free(centros);
  free(bcls);
  free(nexcl);
  return 0;
}
