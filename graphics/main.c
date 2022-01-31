#include "../asteroids/asteroids.h"
#include "../c_vector/vector.h"
#include "../geom/dyn_params.h"
#include "../geom/utils.h"
#include "../geom/vec.h"
#include "../threads/ast_params.h"
#include "../threads/bullets_params.h"
#include "../vessel/vessel.h"
#include "actions.h"
#include "gfx.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void *vessel_thread(void *arg) {
    vessel_params *v_b_params = (vessel_params *)arg;
    for (;;) {
        pthread_mutex_lock(&v_b_params->mutex_v2);
        while (!v_b_params->finished) {
            pthread_cond_wait(&v_b_params->cond_start_io,
                              &v_b_params->mutex_v2);
        }
        v_b_params->finished = false;
        pthread_mutex_unlock(&v_b_params->mutex_v2);

        // vessel updates --> via le thread vessel
        vessel_try_fire_bullet(v_b_params->v, v_b_params->bullet,
                               v_b_params->params->bullet_try_fire,
                               v_b_params->params->bullet_max_distance,
                               v_b_params->params->bullet_vel,
                               v_b_params->params->dt);
        vessel_reset_acceleration(v_b_params->v);
        vessel_update_acceleration_periodic(v_b_params->v,
                                            v_b_params->params->vessel_ang_acc,
                                            v_b_params->params->vessel_lin_acc);
        vessel_move_periodic(
            v_b_params->v, v_b_params->params->dt,
            v_b_params->params->pos_min.x, v_b_params->params->pos_max.x,
            v_b_params->params->pos_min.y, v_b_params->params->pos_max.y);
        bullet_move_periodic_all(
            v_b_params->bullet, v_b_params->params->dt,
            v_b_params->params->pos_min.x, v_b_params->params->pos_max.x,
            v_b_params->params->pos_min.y, v_b_params->params->pos_max.y);
        pthread_mutex_lock(&v_b_params->mutex_v1);
        v_b_params->vessle_finish = true;
        pthread_cond_signal(&v_b_params->cond_v1);
        pthread_mutex_unlock(&v_b_params->mutex_v1);
        if (read_game_ended(v_b_params->params)) {
            break;
        }
    }
    return NULL;
}

void *ast_thread_fn(void *arg0) {
    ast_params *ap = (ast_params *)arg0;
    for (;;) {
        asteroid_reset_acceleration_all(*ap->ast);
        asteroid_update_acceleration_periodic_all(
            *ap->ast, ap->dp->grav, ap->dp->repulse, ap->dp->pos_min.x,
            ap->dp->pos_max.x, ap->dp->pos_min.y, ap->dp->pos_max.y);

        pthread_mutex_lock(&ap->dp->mutex_render);
        while (!ap->dp->ast_render_finished) {
            pthread_cond_wait(&ap->dp->cond_render, &ap->dp->mutex_render);
        }
        ap->dp->ast_render_finished = false;
        pthread_mutex_unlock(&ap->dp->mutex_render);

        asteroid_update_position_all_periodic(
            *ap->ast, ap->dp->dt, ap->dp->pos_min.x, ap->dp->pos_max.x,
            ap->dp->pos_min.y, ap->dp->pos_max.y);

        pthread_mutex_lock(&ap->dp->mutex_update);
        ap->dp->counter_update += 1;
        pthread_cond_signal(&ap->dp->cond_update);
        pthread_mutex_unlock(&ap->dp->mutex_update);

        if (read_game_ended(ap->dp)) {
            break;
        }
    }
    return NULL;
}

void *bullets_thread_fn(void *arg0) {
    bullets_params *bp = (bullets_params *)(arg0);
    for (;;) {
        pthread_mutex_lock(&bp->dp->mutex_render);
        while (!bp->dp->blt_render_finished) {
            pthread_cond_wait(&bp->dp->cond_render, &bp->dp->mutex_render);
        }
        bp->dp->blt_render_finished = false;
        pthread_mutex_unlock(&bp->dp->mutex_render);

        bullet_move_periodic_all(bp->bullets, bp->dp->dt, bp->dp->pos_min.x,
                                 bp->dp->pos_max.x, bp->dp->pos_min.y,
                                 bp->dp->pos_max.y);
        bullet_destroy_after_travel(bp->bullets);

        pthread_mutex_lock(&bp->dp->mutex_update);
        bp->dp->counter_update += 1;
        pthread_cond_signal(&bp->dp->cond_update);
        pthread_mutex_unlock(&bp->dp->mutex_update);
        if (read_game_ended(bp->dp)) {
            break;
        }
    }
    return NULL;
}

/// Render some white noise.
/// @param context graphical context to use.
static void render(struct gfx_context_t *context, vector asteroids, vessel *v,
                   vector bullets, double x0, double x1, double y0, double y1) {
    gfx_clear(context, COLOR_BLACK);

    uint32_t color = MAKE_COLOR(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);
    for (int i = 0; i < vector_length(&asteroids); i++) {
        asteroid *ast = vector_get(&asteroids, i);
        vec pos = ast->pos;
        double r = ast->r;

        gfx_draw_circle(context, pos, r, color, x0, x1, y0, y1);
    }

    for (int i = 0; i < vector_length(&bullets); i++) {
        bullet *b = vector_get(&bullets, i);

        gfx_draw_dot(context, b->pos, color, x0, x1, y0, y1);
    }

    if (vessel_is_invincible(v)) {
        color = MAKE_COLOR(COLOR_RED, COLOR_RED, COLOR_RED);
    }
    gfx_draw_triangle(context, vessel_to_triangle(v), color, x0, x1, y0, y1);
}

/// Program entry point.
/// @return the application status code (0 if success).
int main() {

    struct gfx_context_t *ctxt =
        gfx_create("Example", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!ctxt) {
        fprintf(stderr, "Graphics initialization failed!\n");
        return EXIT_FAILURE;
    }

    dyn_params params = dyn_params_create_default();
    int num_asteroids = 4;

    vector ast = asteroid_create_random_non_overlaping_asteroids(
        params.asteroid_radius, params.asteroid_vel, params.asteroid_mass,
        params.asteroid_max_vel, params.dt, num_asteroids, params.pos_min.x,
        params.pos_max.x, params.pos_min.y, params.pos_max.y);

    vessel v =
        vessel_create(params.vessel_pos, params.vessel_base_length,
                      params.vessel_mass, params.vessel_max_vel,
                      params.vessel_max_ang_vel, params.vessel_remaining_lifes,
                      params.vessel_inv_time, params.vessel_fire_cooldown_time);
    vector bullets;
    vector_init(&bullets);

    //--> initialise la strucuture contenan t les info pour le threads vessel
    vessel_params v_b_params =
        create_thread_v_b_params(ctxt, &bullets, &v, &params);

    pthread_t ast_thread = {0};
    ast_params ap = ast_params_create(&ast, &bullets, &params);
    pthread_create(&ast_thread, NULL, ast_thread_fn, (void *)&ap);

    pthread_t bullets_thread = {0};
    bullets_params bp = bullets_params_create(&bullets, &params);
    pthread_create(&bullets_thread, NULL, bullets_thread_fn, (void *)&bp);

    pthread_t vessel_threads;
    pthread_create(&vessel_threads, NULL, vessel_thread, (void *)&v_b_params);
    
    while (!params.game_ended) {
        // vessel updates
        pthread_mutex_lock(&v_b_params.mutex_v2);
        v_b_params.finished = true;
        actions_params_from_action(&params, gfx_interpret_key(gfx_keypressed()));
        pthread_cond_signal(&v_b_params.cond_start_io);
        pthread_mutex_unlock(&v_b_params.mutex_v2);
    
        pthread_mutex_lock(&params.mutex_render);
        render(ctxt, ast, &v, bullets, params.pos_min.x, params.pos_max.x, params.pos_min.y, params.pos_max.y);
        
        pthread_mutex_lock(&v_b_params.mutex_v1);
        while(!v_b_params.vessle_finish){
            pthread_cond_wait(&v_b_params.cond_v1, &v_b_params.mutex_v1);
        }
        v_b_params.vessle_finish = false;
        pthread_mutex_unlock(&v_b_params.mutex_v1);
        
        gfx_present(ctxt);
    
        asteroid_blown_by_bullets(&ast, &bullets, params.dt);
        vessel_blown_by_asteroids(&v, ast);
    
        params.ast_render_finished = true;
        params.blt_render_finished = true;
    
        pthread_cond_broadcast(&params.cond_render);
        pthread_mutex_unlock(&params.mutex_render);
    
    
        if (vector_length(&ast) == 0) {
            printf("Game over: you won.\n");
            write_game_ended(&params, true);
            break;
        }
    
        if (v.remaining_lifes == 0) {
            printf("Game over: you lost.\n");
            write_game_ended(&params, true);
            break;
        }
    
        pthread_mutex_lock(&params.mutex_update);
        while (params.counter_update != 2) {
            pthread_cond_wait(&params.cond_update, &params.mutex_update);
        }
        params.counter_update = 0;
        pthread_mutex_unlock(&params.mutex_update);
    }
    
    pthread_join(vessel_threads, NULL);
    pthread_join(ast_thread, NULL);
    pthread_join(bullets_thread, NULL);

    vector_free(&ast);
    vector_free(&bullets);

    gfx_destroy(ctxt);
    return EXIT_SUCCESS;
}
