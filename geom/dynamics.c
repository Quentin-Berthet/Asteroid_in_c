#include "utils.h"
#include "vec.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

// v = (p-p_m1) / dt
vec dynamics_compute_vel(vec p, vec p_m1, double dt) {
    vec v = vec_sub(p, p_m1);

    vec_scale_inplace(&v, 1.0 / dt);

    return v;
}

double dynamics_compute_vel_scalar(double p, double p_m1, double dt) {
    return (p - p_m1) / dt;
}

// p = dt * v + p_m1
vec dynamics_get_pos_from_vel(vec p_m1, vec v, double dt) {
    vec v_dt = vec_scale(v, dt);
    vec_add_inplace(&v_dt, p_m1);

    return v_dt;
}

// p_m1 = p - dt * v
vec dynamics_get_pos_m1_from_vel(vec p, vec v, double dt) {
    vec v_dt = vec_scale(v, -dt);
    vec_add_inplace(&v_dt, p);

    return v_dt;
}

void dynamics_verlet_inplace(vec *p, vec *p_m1, vec acc, double dt) {
    vec p_tmp = *p;

    vec_scale_inplace(p, 2.0);
    vec_sub_inplace(p, *p_m1);
    vec_add_inplace(p, vec_scale(acc, dt * dt));

    *p_m1 = p_tmp;
}

void dynamics_verlet_no_acc_inplace(vec *p, vec *vel, double dt) {
    vec next = vec_scale(*vel, dt);
    vec_add_inplace(p, next);
}

vec dynamics_verlet(vec p, vec p_m1, vec acc, double dt) {
    dynamics_verlet_inplace(&p, &p_m1, acc, dt);
    return p;
}

void dynamics_verlet_limited_inplace(vec *p, vec *p_m1, vec acc, double dt,
                                     double max_vel) {
    dynamics_verlet_inplace(p, p_m1, acc, dt);
    vec vel = dynamics_compute_vel(*p, *p_m1, dt);
    if (vec_norm(vel) > max_vel) {
        *p = vec_add(*p_m1, vec_scale(vec_unit(vel), dt * max_vel));
    }
}

void dynamics_verlet_scalar_inplace(double *p, double *p_m1, double acc,
                                    double dt) {
    double p_tmp = *p;
    *p *= 2.0;
    *p -= *p_m1;
    *p += dt * dt * acc;

    *p_m1 = p_tmp;
}

void dynamics_verlet_scalar_limited_inplace(double *p, double *p_m1, double acc,
                                            double dt, double max_vel) {
    dynamics_verlet_scalar_inplace(p, p_m1, acc, dt);
    double vel = dynamics_compute_vel_scalar(*p, *p_m1, dt);
    if (fabs(vel) > max_vel) {
        *p = *p_m1 + dt * max_vel * double_sign(vel);
    }
}

void dynamics_make_periodic(vec *p, double x0, double x1, double y0,
                            double y1) {
    assert(x1 > x0 && "Periodicity implies x1 > x0");
    assert(y1 > y0 && "Periodicity implies y1 > y0");
    p->x = p->x > x1 ? x0 + (p->x - x1) : p->x;
    p->y = p->y > y1 ? y0 + (p->y - y1) : p->y;
    p->x = p->x < x0 ? x1 - (x0 - p->x) : p->x;
    p->y = p->y < y0 ? y1 - (y0 - p->y) : p->y;
}

void dynamics_make_positions_periodic(vec *p, vec *p_m1, double x0, double x1,
                                      double y0, double y1) {
    assert(x1 > x0 && "Periodicity implies x1 > x0");
    assert(y1 > y0 && "Periodicity implies y1 > y0");

    if (p->x > x1) {
        p->x += x0 - x1;
        p_m1->x += x0 - x1;
    }

    if (p->y > y1) {
        p->y += y0 - y1;
        p_m1->y += y0 - y1;
    }

    if (p->x < x0) {
        p->x += x1 - x0;
        p_m1->x += x1 - x0;
    }

    if (p->y < y0) {
       p->y += y1 - y0;
        p_m1->y += y1 - y0;
    }
}

vec dynamics_compute_grav_force(double grav, double m1, double m2, vec p1,
                                vec p2) {
    vec p1_p2 = vec_distance_vec(p1, p2);
    double r = vec_norm(p1_p2);
    assert(r > 1.0e-15);

    vec_scale_inplace(&p1_p2, 1.0 / r);
    double grav_fact = grav * m1 * m2 / (r * r);
    vec grav_force = vec_scale(p1_p2, grav_fact);

    return grav_force;
}

vec dynamics_compute_grav_force_periodic(double grav, double m1, double m2,
                                         vec p1, vec p2, double x0, double x1,
                                         double y0, double y1) {
    vec p1_p2 = vec_distance_periodic_vec(p1, p2, x0, x1, y0, y1);
    double r = vec_norm(p1_p2);
    assert(r > 1.0e-15);

    vec_scale_inplace(&p1_p2, 1.0 / r);
    double grav_fact = grav * m1 * m2 / (r * r);
    vec grav_force = vec_scale(p1_p2, grav_fact);

    return grav_force;
}

vec dynamics_compute_repulsive_force(double repulse, double rm, vec p1,
                                     vec p2) {
    vec p1_p2 = vec_distance_vec(p1, p2);
    double r = vec_norm(p1_p2);
    assert(r > 1.0e-15);

    vec_scale_inplace(&p1_p2, 1.0 / r);

    double rep_fact = -repulse * pow(rm / r, 20);
    vec rep_force = vec_scale(p1_p2, rep_fact);

    return rep_force;
}

vec dynamics_compute_repulsive_force_periodic(double repulse, double rm, vec p1,
                                              vec p2, double x0, double x1,
                                              double y0, double y1) {
    vec p1_p2 = vec_distance_periodic_vec(p1, p2, x0, x1, y0, y1);
    double r = vec_norm(p1_p2);
    assert(r > 1.0e-15);

    vec_scale_inplace(&p1_p2, 1.0 / r);

    double rep_fact = -repulse * pow(rm / r, 20);
    vec rep_force = vec_scale(p1_p2, rep_fact);

    return rep_force;
}
