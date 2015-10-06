#include "utils.h"

int pertence(int x, int *v, int tam) {
  for(int i = 0; i < tam; i++) {
    if(x == v[i])
      return 1;
  }
  return 0;
}

float max(float *v, int size) {
  float maior = v[0];
  for(int i = 1; i < size; i++) {
    if(v[i] > maior)
      maior = v[i];
  }
  return maior;
}

float delta(float xfin, float xini) {
  //return (xfin > xini) ? xfin-xini:xini-xfin;
  return xfin - xini;
}

float distancia(float *v1, float *v2, int n) {
  float d = 0;
  for(int i = 0; i < n; i++)
    d += pow(delta(v2[i], v1[i]), 2.0);
  return d;
}

int olhaChar() {
  int c;
  c = fgetc(stdin);
  if(c != EOF)
    ungetc(c, stdin);
  return c;
}

//void liberaDim2(void **ptr, int n) {
//  for(int i = 0; i < n; i++)
//    free(ptr[i]);
//  free(ptr);
//}
