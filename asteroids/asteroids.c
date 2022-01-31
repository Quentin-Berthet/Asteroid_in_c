#include "asteroids.h"
#include "../c_vector/vector.h"
#include "../geom/dynamics.h"
#include "../geom/utils.h"
#include "../vessel/bullet.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

asteroid *asteroid_create(vec pos, vec pos_m1, vec acc, double r, double mass,
                            double max_velocity, int generation) {
    asteroid *ast = malloc(sizeof(asteroid));
    ast->pos = pos;
    ast->pos_m1 = pos_m1;
    ast->acc = acc;
    ast->r = r;
    ast->mass = mass;
    ast->generation = generation;
    ast->max_velocity = max_velocity;
    return ast;
}

asteroid *asteroid_create_with_velocity(vec pos, double r, vec vel, vec acc,
                                        double mass, double max_velocity,
                                        int generation, double dt) {
    vec pos_m1 = dynamics_get_pos_m1_from_vel(pos, vel, dt);

    return asteroid_create(pos, pos_m1, acc, r, mass, max_velocity, generation);
}

vector asteroid_create_two_asteroids_from_parent(const asteroid *const ast,
                                                    double dt) {
    vector v;
    vector_init(&v);
    double vel_norm = vec_norm(dynamics_compute_vel(ast->pos, ast->pos_m1, dt));
    double theta = double_rand_inrange(0, 2 * PI);
    vec vel = vec_create(cos(theta), sin(theta));

    for (int i = 0; i < 2; ++i) {
        int sign = i == 0 ? 1 : -1;
        vec new_vel = vec_scale(vel, sign * vel_norm);
        vector_push(&v, (void *)asteroid_create_with_velocity(
                            vec_add(ast->pos, vec_scale(vel, sign * ast->r)),
                            ast->r / sqrt(2.0), new_vel, vec_create_zero(),
                            ast->mass / 2.0, ast->max_velocity,
                            ast->generation + 1, dt));
    }

    return v;
}

vector asteroid_blow(asteroid **ast, double dt) {
    vector children;
    if ((*ast)->generation < 2) {
        children = asteroid_create_two_asteroids_from_parent(*ast, dt);
    } else {
        vector_init(&children);
    }
    asteroid_destroy(ast);
    return children;
}

void asteroid_blown_by_bullets(vector *ast, vector *bullets, double dt) {
    for (int i = 0; i < vector_length(bullets); ++i) {
        for (int j = 0; j < vector_length(ast); ++j) {
            if (bullet_is_inside_asteroid(
                    (bullet *)vector_get(bullets, i),
                    (const asteroid *const)vector_get(ast, j))) {
                bullet *b = (bullet *)vector_remove(bullets, i);
                bullet_destroy(&b);
                i -= 1; // a bit ugly but... we removed an element so we must go
                        // back one i
                asteroid *a = (asteroid *)vector_remove(ast, j);
                vector children = asteroid_blow(&a, dt);
                vector_drain_into(&children, ast);
                vector_free(&children);
                break; // break at most one asteroid with a bullet
            }
        }
    }
}

vector asteroid_create_random_non_overlaping_asteroids(
    double radius, double vel_norm, double mass, double max_velocity, double dt,
    int num_asteroids, double x0, double x1, double y0, double y1) {
    vector v;
    vector_init(&v);

    double theta = double_rand_inrange(0, 2 * PI);
    vec vel = vec_create(cos(theta), sin(theta));
    vec new_vel = vec_scale(vel, vel_norm);
    vec pos = vec_create_rand(x0, x1, y0, y1);
    vector_push(&v, (void *)asteroid_create_with_velocity(
                        pos, radius, new_vel, vec_create_zero(), mass,
                        max_velocity, 0, dt));

    for (int i = 1; i < num_asteroids; ++i) {
        theta = double_rand_inrange(0, 2 * PI);
        vel = vec_create(cos(theta), sin(theta));
        new_vel = vec_scale(vel, vel_norm);
        bool in_range = true;
        while (in_range) {
            pos = vec_create_rand(x0, x1, y0, y1);
            for (int ia = 0; ia < vector_length(&v); ++ia) {
                asteroid *ast = (asteroid *)vector_get(&v, ia);
                if (!is_in_circle(ast->pos, ast->r + radius, pos)) {
                    in_range = false;
                    break;
                }
            }
        }

        vector_push(&v, (void *)asteroid_create_with_velocity(
                            pos, radius, new_vel, vec_create_zero(), mass,
                            max_velocity, 0, dt));
    }

    return v;
}

void asteroid_update_position(asteroid *ast, double dt) {
    dynamics_verlet_limited_inplace(&ast->pos, &ast->pos_m1, ast->acc, dt,
                                    ast->max_velocity);
    // dynamics_verlet_inplace(&ast->pos, &ast->pos_m1, ast->acc, dt);
}

void asteroid_make_periodic(
    asteroid *ast, double x0, double x1, double y0,
    double y1) { // TODO add these things as arguments. it's ugly.
    dynamics_make_positions_periodic(&ast->pos, &ast->pos_m1, x0, x1, y0, y1);
}

void asteroid_reset_acceleration(asteroid *a) { a->acc = vec_create_zero(); }

void asteroid_update_acceleration(asteroid *lhs, const asteroid *const rhs,
                                  double grav, double repulse) {
    vec grav_force = dynamics_compute_grav_force(grav, lhs->mass, rhs->mass,
                                                 lhs->pos, rhs->pos);
    vec_scale_inplace(&grav_force, 1.0 / lhs->mass);

    vec_add_inplace(&lhs->acc, grav_force);

    double rm = sqrt(2.0) * (lhs->mass + rhs->mass) /
                (2 * max(lhs->mass, rhs->mass)) * (lhs->r + rhs->r);

    vec rep_force =
        dynamics_compute_repulsive_force(repulse, rm, lhs->pos, rhs->pos);
    vec_scale_inplace(&rep_force, 1.0 / lhs->mass);

    vec_add_inplace(&lhs->acc, rep_force);
}

void asteroid_update_acceleration_periodic(asteroid *lhs,
                                           const asteroid *const rhs,
                                           double grav, double repulse,
                                           double x0, double x1, double y0,
                                           double y1) {
    vec grav_force = dynamics_compute_grav_force_periodic(
        grav, lhs->mass, rhs->mass, lhs->pos, rhs->pos, x0, x1, y0, y1);
    vec_scale_inplace(&grav_force, 1.0 / lhs->mass);

    vec_add_inplace(&lhs->acc, grav_force);

    double rm = sqrt(2.0) * (lhs->mass + rhs->mass) /
                (2 * max(lhs->mass, rhs->mass)) * (lhs->r + rhs->r);

    vec rep_force = dynamics_compute_repulsive_force_periodic(
        repulse, rm, lhs->pos, rhs->pos, x0, x1, y0, y1);
    vec_scale_inplace(&rep_force, 1.0 / lhs->mass);

    vec_add_inplace(&lhs->acc, rep_force);
}

bool asteroid_is_inside(const asteroid *const ast, vec rhs) {
    return is_in_circle(ast->pos, ast->r, rhs);
}

void asteroid_destroy(asteroid **ast) {
    free(*ast);
    *ast = NULL;
}

void asteroid_reset_acceleration_all(vector ast) {
    for (int ia = 0; ia < vector_length(&ast); ++ia) {
        asteroid_reset_acceleration((asteroid *)vector_get(&ast, ia));
    }
}

void asteroid_update_acceleration_all(vector asteroids, double grav,
                                      double repulse) {
    for (int ia = 0; ia < vector_length(&asteroids); ++ia) {
        for (int ib = ia + 1; ib < vector_length(&asteroids); ++ib) {
            asteroid *ast_a = (asteroid *)vector_get(&asteroids, ia);
            asteroid *ast_b = (asteroid *)vector_get(&asteroids, ib);

            asteroid_update_acceleration(ast_a, (const asteroid *const)ast_b,
                                         grav, repulse);
            asteroid_update_acceleration(ast_b, (const asteroid *const)ast_a,
                                         grav, repulse);
        }
    }
}

void asteroid_update_acceleration_periodic_all(vector asteroids, double grav,
                                               double repulse, double x0,
                                               double x1, double y0,
                                               double y1) {
    for (int ia = 0; ia < vector_length(&asteroids); ++ia) {
        for (int ib = ia + 1; ib < vector_length(&asteroids); ++ib) {
            asteroid *ast_a = (asteroid *)vector_get(&asteroids, ia);
            asteroid *ast_b = (asteroid *)vector_get(&asteroids, ib);

            asteroid_update_acceleration_periodic(
                ast_a, (const asteroid *const)ast_b, grav, repulse, x0, x1, y0,
                y1);
            asteroid_update_acceleration_periodic(
                ast_b, (const asteroid *const)ast_a, grav, repulse, x0, x1, y0,
                y1);
        }
    }
}

void asteroid_update_position_all(vector asteroids, double dt) {
    for (int ia = 0; ia < vector_length(&asteroids); ++ia) {
        asteroid_update_position((asteroid *)vector_get(&asteroids, ia), dt);
    }
}

void asteroid_update_position_all_periodic(vector asteroids, double dt,
                                           double x0, double x1, double y0,
                                           double y1) {
    for (int ia = 0; ia < vector_length(&asteroids); ++ia) {
        asteroid_update_position((asteroid *)vector_get(&asteroids, ia), dt);
        asteroid_make_periodic((asteroid *)vector_get(&asteroids, ia), x0, x1,
                               y0, y1);
    }
}
