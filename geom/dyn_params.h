#ifndef _DYN_PARAMS_H_
#define _DYN_PARAMS_H_

#include "../geom/vec.h"
#include <stdbool.h>

typedef struct _dyn_params {
    double dt;
    double grav;
    double repulse;
    vec pos_min;
    vec pos_max;

    double asteroid_radius;
    double asteroid_vel;
    double asteroid_mass;
    double asteroid_max_vel;

    vec vessel_pos;
    double vessel_base_length;
    double vessel_mass;
    double vessel_max_vel;
    double vessel_max_ang_vel;
    double vessel_ang_acc;
    double vessel_delta_ang_acc;
    double vessel_lin_acc;
    double vessel_delta_lin_acc;
    int vessel_remaining_lifes;
    double vessel_inv_time;
    double vessel_fire_cooldown_time;

    double bullet_vel;
    double bullet_max_distance;
    bool bullet_try_fire;

    bool game_ended;
    pthread_mutex_t mutex_game_ended;
    pthread_mutex_t mutex_render;
    pthread_cond_t cond_render;
    bool ast_render_finished;
    bool blt_render_finished;
    int counter_update;
    pthread_cond_t cond_update;
    pthread_mutex_t mutex_update;
} dyn_params;

dyn_params dyn_params_create(
    double dt, double grav, double repulse, vec pos_min, vec pos_max,

    double asteroid_radius, double asteroid_vel, double asteroid_mass,
    double asteroid_max_vel,

    vec vessel_pos, double vessel_base_length, double vessel_mass,
    double vessel_max_vel, double vessel_max_ang_vel, double vessel_ang_acc,
    double vessel_delta_ang_acc, double vessel_lin_acc,
    double vessel_delta_lin_acc, int vessel_remaining_lifes,
    double vessel_inv_time, double vessel_fire_cooldown_time,

    double bullet_vel, double bullet_max_distance, bool bullet_try_fire,

    bool game_ended);

dyn_params dyn_params_create_default();

bool read_game_ended(dyn_params *params);

void write_game_ended(dyn_params *params, bool state);

#endif
