#ifndef _UTILS_H_
#define _UTILS_H_

#include "vec.h"
#include <math.h>
#include <stdbool.h>

// static const double PI = (4.0 * atan(1.0));
#define PI (4.0 * atan(1.0))

double rescale_to_window(int win_x0, int win_x1, double x0, double x1,
                         double val);

bool is_in_circle(vec center, double r, vec pos);

double double_rand_inrange(double r0, double r1);

int int_rand_inrange(int r0, int r1);

double min(double a, double b);
double max(double a, double b);

void int_swap(int *a, int *b);
void double_swap(double *a, double *b);

int double_sign(double a);

#endif
