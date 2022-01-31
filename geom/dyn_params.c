#include "dyn_params.h"

static double dt = 1.0 / 24.0;
static double grav = 0.0;
static double repulse = 3.0e-3;

static vec pos_min = {.x = 0.0, .y = 0.0};
static vec pos_max = {.x = 1.0, .y = 1.0};

static double asteroid_radius = 0.05;
static double asteroid_vel = 0.005;
static double asteroid_max_vel = 0.05;
static double asteroid_mass = 1.0;

static double vessel_max_ang_vel = 0.15;
static vec vessel_pos = {.x = 0.5, .y = 0.5};
static double vessel_base_length = 0.025;
static double vessel_mass = 0.01;
static double vessel_max_vel = 0.02;
static double vessel_ang_acc = 0.0;
static double vessel_delta_ang_acc = 0.5;
static double vessel_lin_acc = 0.0;
static double vessel_delta_lin_acc = 0.03;
static int vessel_remaining_lifes = 2;
static double vessel_inv_time = 2.0;
static double vessel_fire_cooldown_time = 0.1;

static double bullet_vel = 0.05;
static double bullet_max_distance = 1.0;
static bool bullet_try_fire = false;

static bool game_ended = false;

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

    bool game_ended) {
    dyn_params params;
    params.dt = dt;
    params.grav = grav;
    params.repulse = repulse;
    params.pos_min = pos_min;
    params.pos_max = pos_max;

    params.asteroid_radius = asteroid_radius;
    params.asteroid_vel = asteroid_vel;
    params.asteroid_mass = asteroid_mass;
    params.asteroid_max_vel = asteroid_max_vel;

    params.vessel_pos = vessel_pos;
    params.vessel_base_length = vessel_base_length;
    params.vessel_mass = vessel_mass;
    params.vessel_max_vel = vessel_max_vel;
    params.vessel_max_ang_vel = vessel_max_ang_vel;
    params.vessel_ang_acc = vessel_ang_acc;
    params.vessel_delta_ang_acc = vessel_delta_ang_acc;
    params.vessel_lin_acc = vessel_lin_acc;
    params.vessel_delta_lin_acc = vessel_delta_lin_acc;
    params.vessel_remaining_lifes = vessel_remaining_lifes;
    params.vessel_inv_time = vessel_inv_time;
    params.vessel_fire_cooldown_time = vessel_fire_cooldown_time;

    params.bullet_vel = bullet_vel;
    params.bullet_max_distance = bullet_max_distance;
    params.bullet_try_fire = bullet_try_fire;

    params.game_ended = game_ended;
    params.counter_update = 0;
    
    params.ast_render_finished = false;
    params.blt_render_finished = false;
    
    pthread_mutex_init(&params.mutex_render, NULL);
    pthread_mutex_init(&params.mutex_game_ended, NULL);
    pthread_cond_init(&params.cond_render, NULL);
    
    pthread_mutex_init(&params.mutex_update, NULL);
    pthread_cond_init(&params.cond_update, NULL);
    
    return params;
}

dyn_params dyn_params_create_default() {
    return dyn_params_create(
        dt, grav, repulse, pos_min, pos_max,

        asteroid_radius, asteroid_vel, asteroid_mass, asteroid_max_vel,

        vessel_pos, vessel_base_length, vessel_mass, vessel_max_vel,
        vessel_max_ang_vel, vessel_ang_acc, vessel_delta_ang_acc,
        vessel_lin_acc, vessel_delta_lin_acc, vessel_remaining_lifes,
        vessel_inv_time, vessel_fire_cooldown_time,

        bullet_vel, bullet_max_distance, bullet_try_fire,

        game_ended);
}

bool read_game_ended(dyn_params *params) {
    pthread_mutex_lock(&params->mutex_game_ended);
    bool _game_ended = params->game_ended;
    pthread_mutex_unlock(&params->mutex_game_ended);
    return _game_ended;
}

void write_game_ended(dyn_params *params, bool state) {
    pthread_mutex_lock(&params->mutex_game_ended);
    params->game_ended = state;
    pthread_mutex_unlock(&params->mutex_game_ended);
}
