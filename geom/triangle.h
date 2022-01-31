#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "vec.h"

typedef struct _triangle {
    vec v1, v2, v3;
} triangle;

triangle triangle_create(vec v1, vec v2, vec v3);

void triangle_translate_inplace(triangle *lhs, vec v);
triangle triangle_translate(triangle lhs, vec v);

triangle triangle_rotate(triangle lhs, double phi);
triangle triangle_rotate_around(triangle lhs, double phi, vec center);

double triangle_surface(triangle t);

vec triangle_center_of_gravity(triangle t);

void triangle_print(triangle v);
void triangle_print_id(triangle v, char *id);

#endif
