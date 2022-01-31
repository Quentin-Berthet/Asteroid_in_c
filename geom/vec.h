#ifndef _VEC_H_
#define _VEC_H_

#include <pthread.h>

typedef struct _vec {
    double x, y;
    pthread_mutex_t mutex;
} vec;

vec vec_create(double x, double y);
vec vec_create_zero();
vec vec_create_rand(double x0, double x1, double y0, double y1);

void vec_add_inplace(vec *lhs, vec rhs);
vec vec_add(vec lhs, vec rhs);

void vec_sub_inplace(vec *lhs, vec rhs);
vec vec_sub(vec lhs, vec rhs);

void vec_scale_inplace(vec *lhs, double a);
vec vec_scale(vec lhs, double a);

double vec_scalar_product(vec lhs, vec rhs);

double vec_cross_product(vec lhs, vec rhs);

double vec_norm_sqr(vec v);
double vec_norm(vec v);

vec vec_distance_vec(vec lhs, vec rhs);
vec vec_distance_periodic_vec(vec lhs, vec rhs, double x0, double x1, double y0,
                              double y1);

void vec_unit_inplace(vec *v);
vec vec_unit(vec v);

double vec_distance(vec lhs, vec rhs);
double vec_distance_periodic(vec lhs, vec rhs, double x0, double x1, double y0,
                             double y1);

vec vec_rotate(vec p, double phi);

void vec_print(vec v);
void vec_print_id(vec v, char *id);

#endif
