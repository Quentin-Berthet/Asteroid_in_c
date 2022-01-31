#ifndef TP_ASTEROIDS_BULLETS_PARAMS_H
#define TP_ASTEROIDS_BULLETS_PARAMS_H

#include "../c_vector/vector.h"
#include "../geom/dyn_params.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct bullets_params {
    vector *bullets;
    dyn_params *dp;
    pthread_mutex_t mutex_update;
    pthread_cond_t cond_update;
    // pthread_barrier_t barrier;
    bool finished;
} bullets_params;

bullets_params bullets_params_create(vector *bullets, dyn_params *dp);

#endif // TP_ASTEROIDS_BULLETS_PARAMS_H
