#include "actions.h"
#include <stdbool.h>
#include <stdio.h>

void actions_params_from_action(dyn_params *params, actions act) {
    switch (act) {
    case exit_game:
        write_game_ended(params, true);
        break;
    case turn_left:
        params->vessel_ang_acc += params->vessel_delta_ang_acc;
        break;
    case turn_right:
        params->vessel_ang_acc -= params->vessel_delta_ang_acc;
        break;
    case front_boost:
        params->vessel_lin_acc += params->vessel_delta_lin_acc;
        break;
    case rear_boost:
        params->vessel_lin_acc -= params->vessel_delta_lin_acc;
        break;
    case fire_bullet:
        params->bullet_try_fire = true;
        break;
    case no_action:
        params->vessel_ang_acc = 0.0;
        params->vessel_lin_acc = 0.0;
        params->bullet_try_fire = false;
        break;
    default:
        printf("Error should not be possible to have default action.\n");
        break;
    }
}
