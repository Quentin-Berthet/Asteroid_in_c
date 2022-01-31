#ifndef _ASTEROIDS_H_
#define _ASTEROIDS_H_

#include "../c_vector/vector.h"
#include "../geom/vec.h"
#include <stdbool.h>
#include <pthread.h>


typedef struct _asteroid {
    vec pos;
    vec pos_m1;
    vec acc;
    double r;
    double mass;
    int generation;
    double max_velocity;
    pthread_mutex_t mutex;
} asteroid;

asteroid *asteroid_create(vec pos, vec pos_m1, vec acc, double r, double mass,
                            double max_velocity, int generation);

asteroid *asteroid_create_with_velocity(vec pos, double r, vec vel, vec acc,
                                        double mass, double max_velocity,
                                        int generation, double dt);

vector asteroid_create_two_asteroids_from_parent(const asteroid *const ast, double dt);

vector asteroid_blow(asteroid **ast, double dt);

void asteroid_blown_by_bullets(vector *ast, vector *bullets, double dt);

vector asteroid_create_random_non_overlaping_asteroids(
    double radius, double vel_norm, double mass, double max_velocity, double dt,
    int num_asteroids, double x0, double x1, double y0, double y1);

void asteroid_reset_acceleration(asteroid *a);

void asteroid_update_acceleration(asteroid *lhs, const asteroid *const rhs, double grav, double repulse);

void asteroid_update_acceleration_periodic(asteroid *lhs,
                                           const asteroid *const rhs,
                                            double grav, double repulse,
                                            double x0, double x1, double y0,
                                            double y1);

void asteroid_update_position(asteroid *ast, double dt);

void asteroid_move(asteroid *ast, double dt);

bool asteroid_is_inside(const asteroid *const ast, vec rhs);

void asteroid_destroy(asteroid **ast);

void asteroid_reset_acceleration_all(vector ast);

void asteroid_update_acceleration_all(vector asteroids, double grav,
                                      double repulse);

void asteroid_update_acceleration_periodic_all(vector asteroids, double grav,
                                               double repulse, double x0,
                                               double x1, double y0, double y1);

void asteroid_update_position_all(vector ast, double dt);

void asteroid_update_position_all_periodic(vector asteroids, double dt,
                                           double x0, double x1, double y0,
                                           double y1);

#endif
