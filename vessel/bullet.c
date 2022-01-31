#include "bullet.h"
#include "../c_vector/vector.h"
#include "../geom/dynamics.h"
#include <stdlib.h>
#include <stdio.h>

bullet *bullet_create(vec pos, vec vel, double max_distance) {

    bullet *b = malloc(sizeof(bullet));
    b->pos = pos;
    b->vel = vel;
    b->pos_ini = pos;
    b->current_distance = 0.0;
    b->max_distance = max_distance;
    return b;
}

void bullet_make_periodic(bullet *b, double x0, double x1, double y0,
                          double y1) {
    dynamics_make_periodic(&b->pos, x0, x1, y0, y1);
}

void bullet_update_position(bullet *b, double dt) {
    dynamics_verlet_no_acc_inplace(&b->pos, &b->vel, dt);
}

void bullet_move(bullet *b, double dt) {
    bullet_update_position(b, dt);
    b->current_distance += vec_norm(b->vel) * dt;
}

void bullet_move_periodic(bullet *b, double dt, double x0, double x1, double y0,
                          double y1) {
    bullet_update_position(b, dt);
    b->current_distance += vec_norm(b->vel) * dt;
    bullet_make_periodic(b, x0, x1, y0, y1);
}

void bullet_move_all(vector *bullets, double dt) {
    for (int i = 0; i < vector_length(bullets); ++i) {
        bullet *b = vector_get(bullets, i);
        bullet_move(b, dt);
    }
}

void bullet_move_periodic_all(vector *bullets, double dt, double x0, double x1,
                              double y0, double y1) {
    for (int i = 0; i < vector_length(bullets); ++i) {
        bullet *b = vector_get(bullets, i);
        bullet_move_periodic(b, dt, x0, x1, y0, y1);
    }
}

bool bullet_has_traveled_enough(const bullet *const b) {
    return b->current_distance > b->max_distance;
}

void bullet_destroy_after_travel(vector *bullets) {
    for (int i = 0; i < vector_length(bullets); ++i) {
        bullet *b = vector_get(bullets, i);
        if (bullet_has_traveled_enough(b)) {
            b = (bullet *)vector_remove(bullets, i);
            bullet_destroy(&b);
            i -= 1; // a bit ugly but... we removed an element so we must go
                    // back one i
        }
    }
}

void bullet_destroy(bullet **b) {
    free(*b);
    *b = NULL;
}

bool bullet_is_inside_asteroid(const bullet *const v,
                               const asteroid *const ast) {
    return asteroid_is_inside(ast, v->pos);
}
