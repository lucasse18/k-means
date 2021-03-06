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
  size_t *bcls    = NULL;
  size_t *nexcl   = NULL;
  int *gerados    = NULL;
  double rss      = 0.0;
  //yin yang
  double *cant    = NULL;
  double *ub      = NULL;
  double *lb      = NULL;
  double *var     = NULL;
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

  while((c = getopt_long (argc, argv, "ho:a:k:s:", long_opts, &opt_index)) != -1) {
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

  check(argv[optind], "kmeans: no dataset specified.");

  /* char *final_filename = malloc(sizeof(char) * (2 * strlen(argv[optind])) + 16); */
  /* strcpy(final_filename, "datasets/"); */
  /* strcat(final_filename, argv[optind]); */
  /* strcat(final_filename, "/"); */
  /* strcat(final_filename, argv[optind]); */
  /* strcat(final_filename, ".dat"); */
  /* datafile = fopen(final_filename, "r"); */
  /* check(datafile != NULL, "could not open file %s for reading.", final_filename); */
  /* free(final_filename); */

  datafile = fopen(argv[optind], "r");
  check(datafile != NULL, "could not open file %s for reading.", argv[optind]);

  if(user_seed != -1) {
    srand48(user_seed);
    //104
    log_info("seed: %d", user_seed);
  }
  else {
    long seed = time(NULL);
    srand48(seed);
    log_info("seed: %li", seed);
  }

  Dataset data;
  DATASET_INIT(data, datafile);
  fclose(datafile);

  //FIXME eh necessario tratamento especial para k > data.nex?
  //se usuario nao definiu k, utilizar o criterio de oliveira
  if(k == 0) {
    log_warn("number of clusters not defined, using default value");
    if(data.nex <= 100)
      k = (int) sqrt(data.nex);
    else
      k = (int) (5 * log10(data.nex));
  }

  centros = malloc(k * data.nat * sizeof(double));
  check_mem(centros);
  bcls = malloc(data.nex * sizeof(size_t));
  check_mem(bcls);
  nexcl = malloc(k * sizeof(size_t));
  check_mem(nexcl);
  gerados = malloc(k * sizeof(size_t));
  check_mem(gerados);

  switch(alg) {
    case 1:
      printf("*LLOYD*\n");
      inicializa_naive(data.ex.vec, centros, data.nex, data.nat, k, gerados);
      lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);
      break;

    //TODO algoritmo yinyang
    case 2:
      cant = malloc(k * data.nat * sizeof(double));
      check_mem(cant);
      ub = malloc(data.nex * sizeof(double));
      check_mem(ub);
      lb = malloc(data.nex * sizeof(double));
      check_mem(lb);
      var = malloc(k * sizeof(double));
      check_mem(var);

      printf("*YINYANG*\n");
      inicializa_naive(data.ex.vec, centros, data.nex, data.nat, k, gerados);
      yinyang(data.ex.vec, centros, cant, ub, lb, var, data.nex, data.nat, k,
              bcls, nexcl, &rss);

      free(cant);
      free(ub);
      free(lb);
      free(var);
      free(gerados);
      break;

    //TODO algoritmo kmeanspp
    case 3:
      dist = malloc(data.nex * sizeof(double));
      check_mem(dist);

      inicializa_PP(data.ex.vec, centros, data.nex, data.nat, (size_t)k, gerados, dist);
      lloyd(data.ex.vec, centros, data.nex, data.nat, (size_t)k, bcls, nexcl, &rss);

      free(dist);
      break;

    default:
      abort();
  }

  for(int i = 0; i < k; i++)
    printf("exemplos no cluster [%d]: %zd\n", i, nexcl[i]);

  for (size_t i = 0; i < data.nex; i++) {
    printf("%zd ",bcls[i]);
  }
  printf("\n");

  DATASET_FREE(data);

  if(outfile != stdout) fclose(outfile);
  free(centros);
  free(bcls);
  free(nexcl);

  return 0;

  //liberar recursos e sair em caso de erro
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
  if(dist     != NULL)   free(dist);
  exit(1);
}

void print_usage() {
  printf("Usage: kmeans [options] file...\n");
  printf("Options:\n");
  printf("  -a, --algorithm <ll,yy,pp> Algorithm to be used for clustering.\n");
  printf("  -k, --clusters <arg>       Number of clusters to create.\n");
  printf("  -s, --seed <arg>           Seed used by the initialization algorithm.\n");
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
