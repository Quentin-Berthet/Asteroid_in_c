#ifndef TP_ASTEROIDS_AST_PARAMS_H
#define TP_ASTEROIDS_AST_PARAMS_H

#include "../c_vector/vector.h"
#include "../geom/dyn_params.h"

typedef struct ast_params {
    vector *ast;
    vector *bullets;
    dyn_params *dp;
    pthread_mutex_t mutex_update;
    pthread_cond_t cond_update;
    // pthread_barrier_t barrier;
    bool finished;
} ast_params;

ast_params ast_params_create(vector *ast, vector *bullets, dyn_params *dp);

#endif // TP_ASTEROIDS_AST_PARAMS_H
