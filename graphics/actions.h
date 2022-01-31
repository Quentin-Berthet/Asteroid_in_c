#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "../geom/dyn_params.h"

typedef enum {
    exit_game,
    turn_left,
    turn_right,
    front_boost,
    rear_boost,
    fire_bullet,
    no_action
} actions;

void actions_params_from_action(dyn_params *params, actions act);

#endif
