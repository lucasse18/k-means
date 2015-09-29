#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {

  char *separador=",";
  int tamSeparador = 1;
  int nLinhas = 9;
  int nColunas = 4;
  unsigned posatr;

  double **exemplos = (double **)calloc(nLinhas, sizeof(double *));
  for(int i = 0; i < nLinhas; i++)
    exemplos[i] = (double *)calloc(nColunas, sizeof(double));

  char *aux = (char *)calloc(BUFSIZ, sizeof(char));
  char *atributo = (char *)calloc(64, sizeof(char));
  for(unsigned i = 1, posaux = 0; i <= nLinhas; i++, posaux = 0) {
    fgets(aux, BUFSIZ, stdin);
    if(aux[0] != '%') {
      for(unsigned j = 0; j < nColunas; j++) {
        posatr = 0;
        while(aux[posaux] != separador[0]) {
          if(posatr < 1000)
            printf("%d ", posatr);
          atributo[posatr] = aux[posaux];
          posaux++; posatr++;
        }
        printf("acabou while\n");
        atributo[posatr] = '\0';
        exemplos[i][j] = atof(atributo);
        fprintf(stdout, "%g%s", exemplos[i][j], separador);
        for(unsigned k = 0; k < tamSeparador; k++)
          posaux++;//ignora separador
      }
      posatr = 0;
      while(aux[posaux] != '\n') {
        printf("%d ", posatr);
        atributo[posatr] = aux[posaux];
        posaux++; posatr++;
      }
      atributo[posatr] = '\0';
      //fprintf(stdout, "%s\n", atributo);//teste
    }
    else
      fprintf(stdout, "comentario detectado\n");
  }

  for(int i = 0; i < nLinhas; i++) {
    free(exemplos[i]);
  }
  free(exemplos);

  free(aux);

  return 0;
}
