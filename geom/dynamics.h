#ifndef _DYNAMICS_H_
#define _DYNAMICS_H_

#include "vec.h"

vec dynamics_compute_vel(vec p, vec p_m1, double dt);

double dynamics_compute_vel_scalar(double p, double p_m1, double dt);

vec dynamics_get_pos_from_vel(vec p_m1, vec v, double dt);

// p_m1 = p - dt * v
vec dynamics_get_pos_m1_from_vel(vec p, vec v, double dt);

// Verlet Dynamics see
// https://en.wikipedia.org/wiki/Verlet_integration

void dynamics_verlet_inplace(vec *p, vec *p_m1, vec acc, double dt);

void dynamics_verlet_limited_inplace(vec *p, vec *p_m1, vec acc, double dt, double max_vel);

void dynamics_verlet_no_acc_inplace(vec *p, vec *vel, double dt);

void dynamics_verlet_scalar_inplace(double *p, double *p_m1, double acc, double dt);

void dynamics_verlet_scalar_limited_inplace(double *p, double *p_m1, double acc, double dt, double max_vel);

void dynamics_make_periodic(vec *p, double x0, double x1, double y0, double y1);

void dynamics_make_positions_periodic(vec *p, vec *p_m1, double x0, double x1, double y0, double y1);

vec dynamics_compute_grav_force(double grav, double m1, double m2, vec p1, vec p2);

vec dynamics_compute_grav_force_periodic(double grav, double m1, double m2, vec p1, vec p2, double x0, double x1, double y0, double y1);

vec dynamics_compute_repulsive_force(double repulse, double rm, vec p1, vec p2);

vec dynamics_compute_repulsive_force_periodic(double repulse, double rm, vec p1, vec p2, double x0, double x1, double y0, double y1);

#endif
