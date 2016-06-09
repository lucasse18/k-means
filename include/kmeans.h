#ifndef KMEANS_H
#define KMEANS_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stddef.h>

/* ex      = nex * nat * double
 * cen     = k   * nat * double
 * nex     = size_t
 * nat     = size_t
 * k       = size_t
 * bcls    = nex * size_t
 * nexcl   = k   * size_t
 * rss     = k   * double
 * gen     = k   * int
 * cant    = k   * nat * double
 * ub      = nex * double
 * lb      = nex * double
 * var     = k   * double
 * secbcls = nex * size_t
 * dist    = nex * double
 */

void lloyd(double *ex, double *c, size_t nex, size_t nat,
           size_t k, size_t *bcls, size_t *nexc, double *rss);

void yinyang(double *ex, double *c, double *cant, double *ub,
             double *lb, double *var, size_t nex, size_t nat, size_t k,
             size_t *bcls, size_t *nexcl, double *rss, double lb_mult);

void naive_init(double *ex, double *c, size_t nex,
                size_t nat, size_t k, int *gen);

void inicializa_PP(double *ex, double *c, size_t nex,
                   size_t nat, size_t k, int *gen, double *dist);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //KMEANS_H
