#ifndef _VESSEL_H_
#define _VESSEL_H_

#include "../asteroids/asteroids.h"
#include "../c_vector/vector.h"
#include "../geom/dyn_params.h"
#include "../geom/triangle.h"
#include "../geom/vec.h"
#include "bullet.h"
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

// the vessel will have base "1" height "3/2" (in "base" units) and being
// isocele. phi is the angle between the "height" and the horizontal axis. It is
// of pi/2 initially. the vectors joining its centroid for phi = pi / 2 and the
// vertices are (1, 0), (-1/2, -1/2), (1/2, -1/2).

typedef struct _vessel {
    vec pos;
    vec pos_t1;
    double phi;    // angle
    double phi_t1; // angle
    vec acc;
    double ang_acc;
    double base_length;
    double mass;
    double max_velocity;
    double max_ang_velocity;
    int remaining_lifes;
    struct timespec start_time;
    struct timespec last_fire_time;
    double inv_time;
    double fire_cooldown_time;
} vessel;

typedef struct _vessel_params{
    struct gfx_context_t *ctxt;
    pthread_mutex_t mutex_v2;
    pthread_mutex_t mutex_v1;
    pthread_cond_t cond_v1;
    pthread_cond_t cond_start_io;
    bool vessle_finish;
    bool finished;
    vector *bullet;
    vessel *v;
    dyn_params *params;
} vessel_params;

static const vec tip = {.x = 0.0, .y = 1.0};
static const vec left = {.x = -0.5, .y = -0.5};
static const vec right = {.x = 0.5, .y = -0.5};

vessel_params create_thread_v_b_params( struct gfx_context_t *ctxt , vector *bullet, vessel *v, dyn_params *params);

vessel vessel_create(vec pos, double base_length, double mass,
                     double max_velocity, double max_ang_velocity,
                     int remaining_lifes, double inv_time,
                     double fire_cooldown_time);

void vessel_make_periodic(vessel *v, double x0, double x1, double y0,
                          double y1);

void vessel_reset_acceleration(vessel *a);

void vessel_update_angular_acceleration(vessel *v, double);

void vessel_update_grav_acceleration(vessel *v, const asteroid *const rhs,
                                     double grav);

void vessel_update_grav_acceleration_periodic(vessel *v,
                                              const asteroid *const rhs,
                                              double grav, double x0, double x1,
                                              double y0, double y1);

void vessel_update_acceleration_periodic(vessel *v, double delta_a,
                                         double force);


void vessel_update_position(vessel *v, double dt);

void vessel_update_position_periodic(vessel *v, double dt);

void vessel_move_periodic(vessel *v, double dt, double x0, double x1, double y0,
                          double y1);

void vessel_blown(vessel *v);

bool vessel_is_inside_asteroid(const vessel *const v, asteroid *ast);

void vessel_blown_by_asteroids(vessel *v, vector asteroids);

bool vessel_is_invincible(vessel *v);

bool vessel_can_fire(const vessel *const v);

bullet *vessel_fire_bullet(vessel *v, double max_distance, double bullet_vel,
                           double dt);

void vessel_try_fire_bullet(vessel *v, vector *bullets, bool bullet_try_fire,
                            double bullet_max_distance, double bullet_vel,
                            double dt);

triangle vessel_to_triangle(const vessel *const v);

#endif
