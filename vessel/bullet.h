#ifndef _BULLET_H_
#define _BULLET_H_

#include "../asteroids/asteroids.h"
#include "../c_vector/vector.h"
#include "../geom/triangle.h"
#include "../geom/vec.h"
#include <stdbool.h>
#include <time.h>

typedef struct _bullet {
    vec pos;
    vec vel;
    vec pos_ini;
    double current_distance;
    double max_distance;
} bullet;

bullet *bullet_create(vec pos, vec vel, double max_distance);

void bullet_make_periodic(bullet *v, double x0, double x1, double y0,
                          double y1);

void bullet_update_position(bullet *v, double dt);

void bullet_move(bullet *v, double dt);

void bullet_move_periodic(bullet *v, double dt, double x0, double x1, double y0,
                          double y1);

void bullet_move_all(vector *bullets, double dt);

void bullet_move_periodic_all(vector *bullets, double dt, double x0, double x1,
                              double y0, double y1);

bool bullet_has_traveled_enough(const bullet *const b);

void bullet_destroy_after_travel(vector *bullets);

void bullet_destroy(bullet **v);

bool bullet_is_inside_asteroid(const bullet *const v,
                               const asteroid *const ast);

#endif
