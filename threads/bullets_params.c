#include "bullets_params.h"

bullets_params bullets_params_create(vector *bullets, dyn_params *dp) {
    bullets_params params = (bullets_params){0};
    params.bullets = bullets;
    params.dp = dp;
    pthread_mutex_init(&params.mutex_update, NULL);
    pthread_cond_init(&params.cond_update, NULL);
    // pthread_barrier_init(&params.barrier, NULL, 4);
    params.finished = false;
    return params;
}
