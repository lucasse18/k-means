#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dbg.h>

typedef enum{false = 0, true} bool;

//Iteration type
typedef struct iteration {
  int calculated;
  char *result;
} itr_t;

itr_t *itr_init(size_t size) {
  itr_t *itr = malloc(sizeof(itr_t));
  check_mem(itr);
  itr->result = malloc(size * sizeof(char));
  check_mem(itr->result);
  itr->calculated = 0;
  return itr;
 error:
  return NULL;
}

itr_t *itr_destroy(itr_t *itr) {
  if(itr != NULL) {
    if(itr->result != NULL)
      free(itr->result);
    free(itr);
  }
  return NULL;
}

//Processed result type
typedef struct result {
  double gain;
  size_t errors;
} res_t;

res_t *res_init() {
  res_t *new = malloc(sizeof(res_t));
  check_mem(new);
  new->errors = 0;
  new->gain = 0.0;
  return new;
 error:
  return NULL;
}

res_t *res_destroy(res_t *res) {
  if(res != NULL)
    free(res);
  return NULL;
}

res_t *res_clear(res_t *res) {
  res->errors = 0;
  res->gain = 0.0;
  return res;
}

void res_write_out(res_t *res, FILE *out) {
  fprintf(out, "%4.0f, %2zd\n", res->gain, res->errors);
}

int main(int argc, char *argv[]) {
  check(argc > 4, "usage: process <out1> <out2> <used dataset # of examples> <used lb_mult>");

  FILE *yy_file = fopen(argv[1], "r");
  check(yy_file, "could not open file %s for reading", argv[1]);

  FILE *yy_mult_file = fopen(argv[2], "r");
  check(yy_mult_file, "could not open file %s for reading", argv[2]);

  //linha contem um numero para cada exemplo, entao precisa de nex caracteres
  size_t nex = (size_t) atol(argv[3]);
  //espaco para '\n' e '\0'
  size_t line_len = nex + 2;
  
  itr_t *yy = itr_init(line_len);
  itr_t *yy_mult = itr_init(line_len);
  
  res_t *res = res_init();

  fprintf(stdout, "%%lb_mult: %.2f\n", atof(argv[4]));
  fprintf(stdout, "%%ganho erros\n");
  
  while (!feof(yy_file) && !feof(yy_mult_file)) {
    //read yy
    fscanf(yy_file, "%d%*c", &(yy->calculated));
    fgets(yy->result, line_len, yy_file);

    //read yy_mult
    fscanf(yy_mult_file, "%d%*c", &(yy_mult->calculated));
    fgets(yy_mult->result, line_len, yy_mult_file);    

    //compute errors
    res = res_clear(res);
    for(size_t i = 0; i < nex; i++)
      if(yy->result[i] != yy_mult->result[i])
        res->errors++;

    //compute gain
    res->gain = yy->calculated - yy_mult->calculated;

    res_write_out(res, stdout);
  }

  fclose(yy_file);
  fclose(yy_mult_file);
  yy = itr_destroy(yy);
  yy_mult = itr_destroy(yy_mult);
  res = res_destroy(res);
  return 0;

 error:
  exit(1);
}
