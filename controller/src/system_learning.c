#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <physical_model.h>
#include <kohonen.h>
#include <reinforcement.h>



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

int system_learning ( unsigned long int max_duration )
{
    int box, u;
    unsigned long int duration;
    t_state state;
    double new_sr, sr, pr, init_pos, theta_variation, input_kn[4];
	//double init_theta;
    char failure;

    //Simulation initialization
    init_pos = ((double)rand()/RAND_MAX)*(BALL_RIGHT_LIMIT-BALL_LEFT_LIMIT)+
                                                                BALL_LEFT_LIMIT;
//  init_theta = ((double)rand()/RAND_MAX)*(THETA_RIGHT_LIMIT-THETA_LEFT_LIMIT)+
//                                                             THETA_LEFT_LIMIT;
    model_initialize(init_pos*0.8, 0, 0, 0,
//  init_theta,
	0, 0, &state);

    duration = 0;
    ase_clear_eligibility();
    ace_clear_act();

    //Compute the first action
    model_state_copy_kn(input_kn, &state);
    box = kohonen_step (input_kn);
    ace_step(box);
    sr = ace_get(box);
    u = ase_step (box);

    do {
        //Update the physical model
        theta_variation = THETA_SENSIBILITY*u+state.theta;
        model_simulate (&state, TIME_TICK, theta_variation, 0.0);

        //detect failure
        failure = model_failure(&state, duration);

        //Compute next action
        model_state_copy_kn(input_kn, &state);
        box = kohonen_step (input_kn);

        //Learning
        new_sr = ace_get(box);
        pr = (failure==0)?0.:-1.;
        ace_update (new_sr-sr+pr);
        ase_update (new_sr-sr+pr);

        if (failure==0) {
            ace_step(box);
            sr = ace_get(box);
            u = ase_step (box);
        } else {
            ace_step(-1);
            ase_step (-1);
        }

        duration++;
    //exit conditions: failure or max iteration time elapsed
    } while (duration<max_duration && failure==0);

    return duration;    
}

