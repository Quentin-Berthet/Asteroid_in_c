#include "utils.h"
#include <assert.h>
#include <stdlib.h>

double rescale_to_window(int win_x0, int win_x1, double x0, double x1,
                         double val) {
    // TODO show possible errors here. had to comment asserts because failing
    // for triangles. Ugly.

    // assert(x1 > x0 && "x0 must be smaller than x1 for rescaling");
    // assert(win_x1 > win_x0 && "win_x0 must be smaller than win_x1 for
    // rescaling"); assert(val <= x1 && val >= x0 && "val must be in [x0, x1]");

    double rescaled = win_x0 + (val - x0) / (x1 - x0) * (win_x1 - win_x0);

    // assert(rescaled <= win_x1 && val >= win_x0 && "rescaled must be in
    // [win_x0, win_x1]");

    return rescaled;
}

bool is_in_circle(vec center, double r, vec p) {
    return (vec_distance(center, p) <= r);
}

double double_rand_inrange(double r0, double r1) {
    assert(r1 > r0 && "range must be r0 < r1");
    double r = (double)rand() / (double)RAND_MAX;

    r *= (r1 - r0);
    r += r0;

    return r;
}

int int_rand_inrange(int r0, int r1) {
    assert(r1 > r0 && "range must be r0 < r1");
    int delta = r1 - r0;
    int r = rand() % delta;
    r += r0;
    return r;
}

double min(double a, double b) {
    if (a < b) {
        return a;
    }
    return b;
}

double max(double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}

void int_swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void double_swap(double *a, double *b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

int double_sign(double a) {
    if (a > 0) {
        return 1;
    } else if (a < 0) {
        return -1;
    } else {
        return 0;
    }
}
