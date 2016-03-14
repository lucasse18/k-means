#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH BUFSIZ

int main() {

  FILE *data_file = fopen("datasets/iris/iris.arff","r");
  float valor;
  VECTOR_INIT(exemplos);

  char linha[MAX_LINE_LENGTH];
  FILE *string_stream;
  while (!feof(data_file)) {
    fgets(linha, MAX_LINE_LENGTH, data_file);
    if (linha[0] != '%') {
      string_stream = fmemopen(linha, strlen(linha), "r");
      while (fscanf(string_stream, "%f", &valor) == 1) {
        fgetc(string_stream);
        VECTOR_APPEND(exemplos, valor);
      }
      fclose(string_stream);
    }
  }

  printf("\n");

  for (int i = 0; i < VECTOR_SIZE(exemplos); i++)
    printf("%.2f ", VECTOR_GET(exemplos, i));
  printf("\n\n");

  printf("Vector Size: %d\n",VECTOR_SIZE(exemplos));

  VECTOR_FREE(exemplos);
  return 0;
}
