#include "utils.h"
#define BUFSIZ 1024

int main() {

  char *separador=",";

  unsigned nLinhas, nColunas;
  fscanf(stdin,"%d %d", &nLinhas, &nColunas);
  fgetc(stdin);//descarta quebra de linha

  float **exemplos = (float **)calloc(nLinhas, sizeof(float *));
  for(unsigned i = 0; i < nLinhas; i++)
    exemplos[i] = (float *)calloc(nColunas, sizeof(float));

  char **grupoVerdadeiro = (char **)calloc(nLinhas, sizeof(char *));
  for(unsigned i = 0; i < nLinhas; i++)
    grupoVerdadeiro[i] = (char *)calloc(BUFSIZ, sizeof(char));

  fprintf(stdout, "Alocou.\n");

  char *aux = calloc(2048, sizeof(char));
  unsigned tamSeparador = (unsigned)strlen(separador);
  for(unsigned i = 0; i < nLinhas; i++) {
    if(olhaChar() != 37) {//37 = %
      for(unsigned j = 0; j < nColunas; j++) {
        fscanf(stdin, "%f", &exemplos[i][j]);
        fprintf(stdout, "%.2f%s", exemplos[i][j], separador);//teste
        for(unsigned k = 0; k < tamSeparador; k++)
          fgetc(stdin);//retira separador
      }
      fgets(grupoVerdadeiro[i], BUFSIZ, stdin);
      fprintf(stdout, "%s", grupoVerdadeiro[i]);//teste
    }
    else {
      fgets(aux, BUFSIZ, stdin);//retira comentario
      i--;
    }
  }

  for(int i = 0; i < nLinhas; i++) {
    free(exemplos[i]);
  }
  free(exemplos);
  free(aux);

  for(unsigned i = 0; i < nLinhas; i++)
    free(grupoVerdadeiro[i]);
  free(grupoVerdadeiro);

  return 0;
}
