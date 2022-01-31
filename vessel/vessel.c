#include "vessel.h"
#include "../geom/dynamics.h"
#include "../geom/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

vessel vessel_create(vec pos, double base_length, double mass,double max_velocity, double max_ang_velocity, int remaining_lifes, double inv_time, double fire_cooldown_time) {
    vessel v;
    clock_gettime(CLOCK_MONOTONIC, &v.start_time);
    v.last_fire_time = v.start_time;
    v.pos = pos;
    v.pos_t1 = v.pos;
    v.phi = 0.0;
    v.phi_t1 = v.phi;
    v.acc = vec_create_zero();
    v.ang_acc = 0.0;
    v.base_length = base_length;
    v.mass = mass;
    v.remaining_lifes = remaining_lifes;
    v.inv_time = inv_time;
    v.fire_cooldown_time = fire_cooldown_time;
    v.max_velocity = max_velocity;
    v.max_ang_velocity = max_ang_velocity;
    return v;
}

vessel_params create_thread_v_b_params(struct gfx_context_t *ctxt, vector *bullet, vessel *v, dyn_params *params){
    vessel_params v_b_p;
    v_b_p.ctxt = ctxt;
    pthread_mutex_init(&v_b_p.mutex_v2, NULL);
    pthread_mutex_init(&v_b_p.mutex_v1, NULL);
    pthread_cond_init(&v_b_p.cond_v1, NULL);
    pthread_cond_init(&v_b_p.cond_start_io, NULL);
    v_b_p.bullet = bullet;
    v_b_p.params = params;
    v_b_p.v = v;
    v_b_p.vessle_finish = false;
    v_b_p.finished = false;
    return v_b_p;
}



void vessel_make_periodic(vessel *v, double x0, double x1, double y0, double y1) {
    dynamics_make_positions_periodic(&v->pos, &v->pos_t1, x0, x1, y0, y1);
}

void vessel_reset_acceleration(vessel *v) {
    v->acc = vec_create_zero();
    v->ang_acc = 0.0;
}

void vessel_update_angular_acceleration(vessel *v, double delta_a) {
    v->ang_acc += delta_a;
}

//  unused!
void vessel_update_grav_acceleration(vessel *v, const asteroid *const rhs, double grav) {
    vec force = dynamics_compute_grav_force(grav, v->mass, rhs->mass, v->pos, rhs->pos);
    vec_scale_inplace(&force, 1.0 / v->mass);
    
    vec_add_inplace(&v->acc, force);
}

void vessel_update_linear_acceleration(vessel *v, double force) {
    vec dir = vec_unit(vec_rotate(tip, v->phi));
    vec_scale_inplace(&dir, force);
    
    vec_add_inplace(&v->acc, dir);
}

//  unused!
void vessel_update_grav_acceleration_periodic(vessel *v, const asteroid *const rhs, double grav, double x0, double x1, double y0, double y1) {
    vec force = dynamics_compute_grav_force_periodic(
            grav, v->mass, rhs->mass, v->pos, rhs->pos, x0, x1, y0, y1);
    vec_scale_inplace(&force, 1.0 / v->mass);
    
    vec_add_inplace(&v->acc, force);
}

void vessel_update_acceleration_periodic(vessel *v, double delta_a, double force) {
    vessel_update_linear_acceleration(v, force);
    vessel_update_angular_acceleration(v, delta_a);
}

void vessel_update_position(vessel *v, double dt) {
    dynamics_verlet_limited_inplace(&v->pos, &v->pos_t1, v->acc, dt, v->max_velocity);
}

void vessel_update_phi(vessel *v, double dt) {
    dynamics_verlet_scalar_limited_inplace(&v->phi, &v->phi_t1, v->ang_acc, dt, v->max_ang_velocity);
}

void vessel_move_periodic(vessel *v, double dt, double x0, double x1, double y0, double y1) {
    vessel_update_position(v, dt);
    vessel_update_phi(v, dt);
    vessel_make_periodic(v, x0, x1, y0, y1);
}

void vessel_blown(vessel *v) {
    if (v->remaining_lifes == 0) {
        printf("We cannot blow a dead vessel");
        return;
    }
    
    *v = vessel_create(v->pos, v->base_length, v->mass, v->max_velocity, v->max_ang_velocity, v->remaining_lifes - 1, v->inv_time, v->fire_cooldown_time);
}

bool vessel_is_inside_asteroid(const vessel *const v, asteroid *ast) {
    return asteroid_is_inside(ast, v->pos);
}

void vessel_blown_by_asteroids(vessel *v, vector asteroids) {
    
    if (!vessel_is_invincible(v)) {
        for (int i = 0; i < vector_length(&asteroids); ++i) {
            asteroid *ast = (asteroid *)vector_get(&asteroids, i);
            if (vessel_is_inside_asteroid(v, ast)) {
                vessel_blown(v);
                return;
            }
        }
    }
}

bool vessel_is_invincible(vessel *v) {
    struct timespec finish_time;
    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    
    double elapsed = (finish_time.tv_sec - v->start_time.tv_sec);
    elapsed += (finish_time.tv_nsec - v->start_time.tv_nsec) / 1000000000.0;
    
    return (elapsed < v->inv_time);
}

bool vessel_can_fire(const vessel *const v) {
    struct timespec finish_time;
    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    
    double elapsed = (finish_time.tv_sec - v->last_fire_time.tv_sec);
    elapsed += (finish_time.tv_nsec - v->last_fire_time.tv_nsec) / 1000000000.0;
    
    return (elapsed > v->fire_cooldown_time);
}

bullet *vessel_fire_bullet(vessel *v, double max_distance, double bullet_vel, double dt) {
    if (vessel_can_fire(v)) {
        vec dir = vec_rotate(tip, v->phi);
        vec bullet_pos =
                vec_add(v->pos, vec_scale(dir, v->base_length)); // tip pos
        vec_unit_inplace(&dir);
        vec vess_vel = dynamics_compute_vel(v->pos, v->pos_t1, dt);
        
        double vel_proj = vec_scalar_product(vess_vel, dir);
        vec vel = vec_scale(dir, vel_proj + bullet_vel);
        
        clock_gettime(CLOCK_MONOTONIC, &v->last_fire_time);
        
        return bullet_create(bullet_pos, vel, max_distance);
    }
    
    return NULL;
}

void vessel_try_fire_bullet(vessel *v, vector *bullets, bool bullet_try_fire, double bullet_max_distance, double bullet_vel, double dt) {
    if (!bullet_try_fire) {
        return;
    }
    bullet *b = vessel_fire_bullet(v, bullet_max_distance, bullet_vel, dt);
    if (b != NULL) {
        vector_push(bullets, b);
    }
}

triangle vessel_to_triangle(const vessel *const v) {
    vec p1 =
            vec_add(v->pos, vec_scale(vec_rotate(tip, v->phi), v->base_length));
    vec p2 =
            vec_add(v->pos, vec_scale(vec_rotate(left, v->phi), v->base_length));
    vec p3 =
            vec_add(v->pos, vec_scale(vec_rotate(right, v->phi), v->base_length));
    
    return triangle_create(p1, p2, p3);
}
