#include "vec.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

static int c[8][2] = {{-1, +1}, {-1, 0}, {-1, -1}, {0, -1},
                      {+1, -1}, {+1, 0}, {+1, +1}, {0, +1}};

vec vec_create(double x, double y) {
    vec v = {.x = x, .y = y};
    pthread_mutex_init(&v.mutex, NULL);
    return v;
}

vec vec_create_zero() { return vec_create(0.0, 0.0); }

vec vec_create_rand(double x0, double x1, double y0, double y1) {
    double x = double_rand_inrange(x0, x1);
    double y = double_rand_inrange(y0, y1);

    return vec_create(x, y);
}

void vec_add_inplace(vec *lhs, vec rhs) {
    lhs->x += rhs.x;
    lhs->y += rhs.y;
}

vec vec_add(vec lhs, vec rhs) {
    vec tmp = lhs;
    vec_add_inplace(&tmp, rhs);
    return tmp;
}

void vec_sub_inplace(vec *lhs, vec rhs) {
    lhs->x -= rhs.x;
    lhs->y -= rhs.y;
}

vec vec_sub(vec lhs, vec rhs) {
    vec tmp = lhs;
    vec_sub_inplace(&tmp, rhs);
    return tmp;
}

void vec_scale_inplace(vec *lhs, double a) {
    //pthread_mutex_lock(&lhs->mutex);
    lhs->x *= a;
    lhs->y *= a;
    //pthread_mutex_unlock(&lhs->mutex);
    
}

vec vec_scale(vec lhs, double a) {
    vec tmp = lhs;
    vec_scale_inplace(&tmp, a);
    return tmp;
}

double vec_scalar_product(vec lhs, vec rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double vec_cross_product(vec lhs, vec rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double vec_norm_sqr(vec v) { return vec_scalar_product(v, v); }

double vec_norm(vec v) { return sqrt(vec_norm_sqr(v)); }

vec vec_distance_vec(vec lhs, vec rhs) { return vec_sub(rhs, lhs); }

vec vec_distance_periodic_vec(vec lhs, vec rhs, double x0, double x1, double y0,
                              double y1) {
    double dist = vec_distance(lhs, rhs);
    vec vec_dist = vec_distance_vec(lhs, rhs);

    double delta_x = x1 - x0;
    double delta_y = y1 - y0;

    for (int i = 0; i < 8; ++i) {
        vec per_pos = vec_create(c[i][0] * delta_x, c[i][1] * delta_y);
        if (dist > vec_distance(lhs, vec_add(rhs, per_pos))) {
            dist = vec_distance(lhs, vec_add(rhs, per_pos));
            vec_dist = vec_distance_vec(lhs, vec_add(rhs, per_pos));
        }
    }
    return vec_dist;
}

void vec_unit_inplace(vec *v) {
    double norm = vec_norm(*v);
    vec_scale_inplace(v, 1.0 / norm);
}

vec vec_unit(vec v) {
    vec tmp = v;
    vec_unit_inplace(&tmp);
    return tmp;
}

double vec_distance(vec lhs, vec rhs) {
    return vec_norm(vec_distance_vec(lhs, rhs));
}

double vec_distance_periodic(vec lhs, vec rhs, double x0, double x1, double y0,
                             double y1) {
    double dist = vec_distance(lhs, rhs);

    double delta_x = x1 - x0;
    double delta_y = y1 - y0;

    for (int i = 0; i < 8; ++i) {
        vec per_pos = vec_create(c[i][0] * delta_x, c[i][1] * delta_y);
        dist = min(dist, vec_distance(lhs, vec_add(rhs, per_pos)));
    }

    return dist;
}

vec vec_rotate(vec p, double phi) {
    double x_prime = p.x * cos(phi) - p.y * sin(phi);
    double y_prime = p.x * sin(phi) + p.y * cos(phi);

    return vec_create(x_prime, y_prime);
}

void vec_print(vec v) { printf("v.x = %f, v.y = %f\n", v.x, v.y); }

void vec_print_id(vec v, char *id) { printf("%s =(%f, %f)\n", id, v.x, v.y); }
