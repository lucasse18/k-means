#include "utils.h"

int pertence(int x, int *v, int tam) {
  for(int i = 0; i < tam; i++) {
    if(x == v[i])
      return 1;
  }
  return 0;
}

double delta(double xfin, double xini) {
  //return (xfin > xini) ? xfin-xini:xini-xfin;
  return xfin - xini;
}

double distancia(double *v1, double *v2, int n) {
  double d = 0;
  for(int i = 0; i < n; i++)
    d += pow(delta(v2[i], v1[i]), 2.0);
  return d;
}

//void liberaDim2(void **ptr, int n) {
//  for(int i = 0; i < n; i++)
//    free(ptr[i]);
//  free(ptr);
//}
