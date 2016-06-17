#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#include "dbg.h"
#include "dataset.h"
#include "kmeans.h"

typedef struct timespec timespec;

int optv = 1;

void print_usage();
int get_alg_code(const char *optarg);
unsigned long rdtsc();

struct timespec timer_start(){
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  return start;
}

//returns time in miliseconds
double timer_end(struct timespec start){
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  double sec = 1000.0 * (end.tv_sec - start.tv_sec);
  double nsec = (end.tv_nsec - start.tv_nsec) / 1000000.0;
  return sec + nsec;
}

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

  static int alg = 1;
  static size_t k = 0;
  static long user_seed = -1;
  static double lb_mult = 1.0;

  static struct option opts[] = {
    {"help",       no_argument,       0, 'h'},
    {"outfile",    required_argument, 0, 'o'},//unused, default stdout
    {"algorithm",  required_argument, 0, 'a'},//default lloyd (1)
    {"clusters",   required_argument, 0, 'k'},//default criterio de oliveira
    {"seed",       required_argument, 0, 's'},//default time(NULL)
    {"mult",       required_argument, 0, 'm'},//default 1.0
    {"quiet",      no_argument,       0, 'q'},
    {"verbose",    no_argument,       0, 'v'},
    {0,           0,                  0,   0}
  };

  int c;
  int index = 0;

  while((c = getopt_long(argc, argv, "ho:a:k:s:m:qv", opts, &index)) != -1) {
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
      k = (size_t) atol(optarg);
      break;

    case 's':
      user_seed = atol(optarg);
      break;

    case 'm':
      lb_mult = atof(optarg);
      break;

    case 'q':
      optv = 0;
      break;

    case 'v':
      optv++;
      break;

    case '?':
      // user passed an unsupported option
      // getopt_long already printed an error message
      exit(1);

    default:
      abort();
    }
  }

  check(argv[optind], "kmeans: no dataset specified, stopping.");
  datafile = fopen(argv[optind], "r");
  check(datafile != NULL, "could not open file %s for reading.", argv[optind]);

  Dataset data;
  DATASET_INIT(data, datafile);
  fclose(datafile);

  //se opcao -s nao foi passada
  if(user_seed == -1)
    user_seed = rdtsc();

  srand48(user_seed);
  printf_v1("seed: %ld\n", user_seed);


  //FIXME eh necessario tratamento especial para k > data.nex?
  //se usuario nao definiu k, utilizar padrao
  if(k == 0) {
    log_warn("number of clusters undefined, using default value");
    if(data.nex <= 100)
      k = (size_t) sqrt(data.nex);
    else
      k = (size_t) (5 * log10(data.nex));
  }

  struct timespec start = timer_start();

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
      printf_v1("*LLOYD*\n");
      naive_init(data.ex.vec, centros, data.nex, data.nat, k, gerados);
      lloyd(data.ex.vec, centros, data.nex, data.nat, k, bcls, nexcl, &rss);
      break;

    case 2:
      printf_v1("*YINYANG*\n");

      cant = malloc(k * data.nat * sizeof(double));
      check_mem(cant);
      ub = malloc(data.nex * sizeof(double));
      check_mem(ub);
      lb = malloc(data.nex * sizeof(double));
      check_mem(lb);
      var = malloc(k * sizeof(double));
      check_mem(var);

      if (lb_mult > 1.0) {
        printf_v1("LB Mult: ");
        printf_v1("%.2f\n", lb_mult);
      }
      naive_init(data.ex.vec, centros, data.nex, data.nat, (size_t) k, gerados);
      yinyang(data.ex.vec, centros, cant, ub, lb, var, data.nex, data.nat,
              (size_t) k, bcls, nexcl, lb_mult);

      free(cant);
      free(ub);
      free(lb);
      free(var);
      free(gerados);

      break;

    case 3:
      //TODO

    default:
      abort();
  }

  printf_v1("clustering time (msec): ");
  printf_v1("%.6lf\n", timer_end(start));

  for(size_t i = 0; i < (unsigned) k; i++)
    printf_v1("cluster [%zd]: %zd samples\n", i, nexcl[i]);

  for(size_t i = 0; i < data.nex; i++)
    printf("%zd",bcls[i]);
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
  printf("  -a, --algorithm <ll,yy,pp>\tAlgorithm to be used for clustering.\n");
  printf("  -k, --clusters <arg>      \tNumber of clusters to create.\n");
  printf("  -s, --seed <arg>          \tSeed used by the initialization algorithm.\n");
  printf("  -m, --mult <arg>          \tNumber to multiply the initial lower bound.\n");
  printf("  -q, --quiet               \tBe more quiet.\n");
  printf("  -v, --verbose             \tBe more verbose.\n");
}

int get_alg_code(const char *optarg) {
  if(strcmp("ll", optarg) == 0)
    return 1;
  if(strcmp("yy", optarg) == 0)
    return 2;
  if(strcmp("pp", optarg) == 0)
    return 3;
  return -1;
}

unsigned long rdtsc(){
    unsigned int lo=0,hi=0;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long)hi << 32) | lo;
}
