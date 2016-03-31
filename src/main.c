#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>

#include "dbg.h"
#include "dataset.h"
#include "kmeans.h"

void print_usage();
int get_alg_code(const char *optarg);

int main(int argc, char *argv[]) {

  //geral
  double *centros = NULL;
  int *bcls       = NULL;
  int *nexcl      = NULL;
  int *gerados    = NULL;
  double rss = 0.0;
  //yin yang
  double *cant    = NULL;
  double *ub      = NULL;
  double *lb      = NULL;
  double *var     = NULL;
  int *secbcls    = NULL;
  //kmeanspp
  double *dist    = NULL;

  FILE *datafile = NULL;
  FILE *outfile  = stdout;
  static int k = 0;
  static int alg = 1;
  static int user_seed = -1;

  static struct option long_opts[] = {
    {"help",      no_argument,       0,          'h'},
    {"outfile",   required_argument, 0,          'o'},//ainda nao usado, default stdout
    {"algorithm", required_argument, 0,          'a'},//default lloyd (1)
    {"clusters",  required_argument, &k,         'k'},//default criterio de oliveira
    {"seed",      required_argument, &user_seed, 's'},//default time(NULL)
    {0,           0,                 0,            0}
  };

  int opt_index = 0;
  int c;

  while((c = getopt_long (argc, argv, "ho:a:k:s:n", long_opts, &opt_index)) != -1) {
    switch(c) {
    case 0:
      break;

    case 'h':
      print_usage();
      goto error;

    case 'o':
      check(outfile == stdout, "duplicate option -o.")
      outfile = fopen(optarg, "w");
      check(outfile != NULL, "could not open file for writing.");
      break;

    case 'a':
      alg = get_alg_code(optarg);
      check(alg > 0, "invalid algorithm.");
      break;

    case 'k':
      k = atoi(optarg);
      check(k > 0, "the number of clusters to create must be greater than zero.");
      break;

      case 's':
      user_seed = atoi(optarg);
      break;

    case '?':
      // user passed an unsupported option
      // getopt_long already printed an error message
      exit(1);

    default:
      abort();
    }
  }

  if(argv[optind] == NULL) {
    fprintf(stderr, "kmeans: no dataset specified.\n");
    goto error;
  }

  datafile = fopen(argv[optind], "r");
  check(datafile != NULL, "could not open file for reading.");

  if(user_seed != -1)
    srand48(user_seed);
  else {
    time_t seed = time(NULL);
    srand48(seed);
    log_info("seed: %li", seed);
  }

  Dataset data;
  DATASET_INIT(data, datafile);
  fclose(datafile);

  //se usuario nao definiu k, utilizar o criterio de oliveira
  //FIXME o que fazer quando k > nex?
  if(k == 0) {
    if(data.nex <= 100)
      k = (int) sqrt(data.nex);
    else
      k = (int) (5 * log10(data.nex));
  }

  log_info("NEX: %d, NAT: %d, K: %d", data.nex, data.nat, k);

  centros = malloc(k * data.nat * sizeof(double));
  check_mem(centros);
  bcls = malloc(data.nex * sizeof(int));
  check_mem(bcls);
  nexcl = malloc(k * sizeof(int));
  check_mem(nexcl);
  gerados = malloc(k * sizeof(int));
  check_mem(gerados);

  switch(alg) {
    case 1:
      printf("*LLOYD*\n");
      inicializa_naive(data.ex.vec, centros, data.nex, data.nat, k, gerados);
      lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);
      break;

    //TODO algoritmo yinyang em progresso(WIP)
    case 2:
      cant = malloc(k * data.nat * sizeof(double));
      check_mem(cant);
      ub = malloc(data.nex * sizeof(double));
      check_mem(ub);
      lb = malloc(data.nex * sizeof(double));
      check_mem(lb);
      var = malloc(k * sizeof(double));
      check_mem(var);
      secbcls = malloc(data.nex * sizeof(int));
      check_mem(secbcls);

      printf("*YINYANG*\n");
      inicializa_naive(data.ex.vec, centros, data.nex, data.nat, k, gerados);
      yinyang(data.ex.vec, centros, cant, ub, lb, var, data.nex,
              data.nat, k, bcls, secbcls, nexcl, &rss);

      free(cant);
      free(ub);
      free(lb);
      free(var);
      free(secbcls);
      break;

    //TODO algoritmo kmeans++ em progresso(WIP)
    case 3:
      dist = malloc(data.nex * sizeof(double));
      check_mem(dist);

      inicializa_PP(data.ex.vec, centros, data.nex, data.nat, k, gerados, dist);
      lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);

      free(dist);
      break;

    default:
      abort();
  }

  for(int i = 0; i < k; i++)
    printf("exemplos no cluster [%d]: %d\n", i, nexcl[i]);

  DATASET_FREE(data);

  if(outfile != NULL && outfile != stdout)
    fclose(outfile);

  free(centros);
  free(bcls);
  free(nexcl);

  return 0;

  error:
  if(outfile  != stdout) fclose(outfile);
  if(datafile != NULL)   fclose(datafile);
  if(centros  != NULL)   free(centros);
  if(bcls     != NULL)   free(bcls);
  if(nexcl    != NULL)   free(nexcl);
  if(gerados  != NULL)   free(gerados);
  if(cant     != NULL)   free(cant);
  if(ub       != NULL)   free(ub);
  if(lb       != NULL)   free(lb);
  if(var      != NULL)   free(var);
  if(secbcls  != NULL)   free(secbcls);
  if(dist     != NULL)   free(dist);
  exit(1);
}

void print_usage() {
  printf("Usage: kmeans [options] file...\n");
  printf("Options:\n");
  printf("  -d, --dataset     Dataset to be clustered.\n");
  printf("  -a, --algorithm   Algorithm to be used in clustering.\n");
  printf("  -k, --clusters    Number of clusters to create.\n");
  printf("  -s, --seed        Seed used in the initialization algorithm.\n");
}

int get_alg_code(const char *optarg) {
  if(strcmp("ll", optarg) == 0)
    return 1;
  if(strcmp("yy", optarg) == 0)
    return 2;
  if(strcmp("pp", optarg) == 0)
    return 3;
  if(strcmp("yypp", optarg) == 0)
    return 4;

  return -1;
}
