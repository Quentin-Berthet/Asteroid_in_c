#include "triangle.h"
#include <math.h>
#include <stdio.h>

triangle triangle_create(vec v1, vec v2, vec v3) {
    triangle t = {.v1 = v1, .v2 = v2, .v3 = v3};
    return t;
}

void triangle_translate_inplace(triangle *lhs, vec v) {
    vec_add_inplace(&lhs->v1, v);
    vec_add_inplace(&lhs->v2, v);
    vec_add_inplace(&lhs->v3, v);
}

triangle triangle_translate(triangle lhs, vec v) {
    triangle t = lhs;
    triangle_translate_inplace(&t, v);
    return t;
}

triangle triangle_rotate(triangle lhs, double phi) {
    vec t_v1 = vec_rotate(lhs.v1, phi);
    vec t_v2 = vec_rotate(lhs.v2, phi);
    vec t_v3 = vec_rotate(lhs.v3, phi);

    return triangle_create(t_v1, t_v2, t_v3);
}

triangle triangle_rotate_around(triangle lhs, double phi, vec center) {
    triangle new_tri = triangle_translate(lhs, center);

    return triangle_rotate(new_tri, phi);
}

double triangle_surface(triangle t) {
    vec e1 = vec_sub(t.v1, t.v2);
    vec e2 = vec_sub(t.v1, t.v3);
    double area = fabs(vec_cross_product(e1, e2)) / 2.0;
    return area;
}

vec triangle_center_of_gravity(triangle t) {
    double new_x = (t.v1.x + t.v2.x + t.v3.x) / 3.0;
    double new_y = (t.v1.y + t.v2.y + t.v3.y) / 3.0;
    return vec_create(new_x, new_y);
}

void triangle_print(triangle t) {
    printf("Triangle = \n");
    vec_print_id(t.v1, "vertex 1 = ");
    vec_print_id(t.v2, "vertex 2 = ");
    vec_print_id(t.v3, "vertex 3 = ");
}

void triangle_print_id(triangle t, char *id) {
    printf("%s\n", id);
    vec_print_id(t.v1, "vertex 1 = ");
    vec_print_id(t.v2, "vertex 2 = ");
    vec_print_id(t.v3, "vertex 3 = ");
}
