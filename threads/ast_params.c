#include "ast_params.h"

ast_params ast_params_create(vector *ast, vector *bullets, dyn_params *dp) {
    ast_params params = (ast_params){0};
    params.ast = ast;
    params.bullets = bullets;
    params.dp = dp;
    pthread_mutex_init(&params.mutex_update, NULL);
    pthread_cond_init(&params.cond_update, NULL);
    // pthread_barrier_init(&params.barrier, NULL, 4);
    params.finished = false;
    return params;
}
