#ifndef KMEANS_H
#define KMEANS_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* ex      = nex * nat * double
 * cen     = k   * nat * double
 * nex     = int
 * nat     = int
 * k       = int
 * bcls    = nex * int
 * nexcl   = k   * int
 * rss     = k   * double
 * gen     = k   * int
 * cant    = k   * nat * double
 * ub      = nex * double
 * lb      = nex * double
 * var     = k   * double
 * secbcls = nex * int
 * dist    = nex * double
 */

void lloyd(double *ex, double *c, int nex, int nat,
           int k, int *bcls, int *nexc, double *rss);

void yinyang(double *ex, double *c, double *cant, double *ub,
             double *lb, double *var, int nex, int nat, int k,
             int *bcls, int *secbcls, int *nexcl, double *rss);

void inicializa_naive(double *ex, double *c, int nex,
                      int nat, int k, int *gen);


void inicializa_PP(double *ex, double *c, int nex,
                   int nat, int k, int *gen, double *dist);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //KMEANS_H
