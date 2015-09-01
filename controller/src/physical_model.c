#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <physical_model.h>
#include <kohonen.h>
#include <reinforcement.h>



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

void model_initialize (double ball_position, double ball_speed, double ball_acl,
                       double theta, double omega, double omegadot,
                       t_state *state)
{
    state->ball_position = ball_position;
    state->ball_speed = ball_speed;
    state->ball_acl = ball_acl;
    state->theta =  theta;
    state->omega = omega;
    state->omegadot = omegadot;
}

void model_simulate ( t_state *state, double delta_t, double newtheta, double F)
{
    t_state ns;
    
    ns.theta = (newtheta < THETA_LEFT_LIMIT) ? THETA_LEFT_LIMIT :
                      ((newtheta > THETA_RIGHT_LIMIT) ? THETA_RIGHT_LIMIT :
                      newtheta);
    //omega = theta' = ( theta(t)-theta(t-T) )/T
    ns.omega = (ns.theta - state->theta) / delta_t;
    //omega' = theta'' = ( theta'(t)-theta'(t-T) )/T = ( omega(t)-omega(t-T) )/T
    ns.omegadot = (ns.omega - state->omega) / delta_t;

    //x'' = -(5/7)*g*sin(theta)-(r+(5/7)*h)*theta''
    ns.ball_acl = -5.0*GRAVITY_ACCELERATION*sin(ns.theta)/7.0;
    ns.ball_acl += -(BALL_RADIUS+(5.0/7.0)*DISTANCE_MOTOR_PLATE)*ns.omegadot;
    ns.ball_acl += F;
    //x' = x''*T+x'(t-T)
    ns.ball_speed = ns.ball_acl * delta_t + state->ball_speed;
    //x = x'*T+x(t-T)
    ns.ball_position = ns.ball_speed * delta_t + state->ball_position;

    if (ns.ball_position <= BALL_LEFT_LIMIT){
        ns.ball_acl = 0.0;
        ns.ball_speed = 0.0;
        ns.ball_position = BALL_LEFT_LIMIT;
    }
    if (ns.ball_position >= BALL_RIGHT_LIMIT){
        ns.ball_acl = 0.0;
        ns.ball_speed = 0.0;
        ns.ball_position = BALL_RIGHT_LIMIT;
    }

    *state = ns;
}

char model_failure ( t_state *state, unsigned long int iteration )
{
    double factor;
    if (MAX_REDUCTION_ITERATIONS>iteration)
        factor = 0.2 + 0.8*(1-(float)iteration/MAX_REDUCTION_ITERATIONS);
    else
        factor = 0.2;

    if (state->ball_position<=(BALL_LEFT_LIMIT*factor) ||
        state->ball_position>=(BALL_RIGHT_LIMIT*factor))
        return 1;

    return 0;
}

void model_state_copy_kn ( double *in, t_state *state )
{
    //the Kohonen networ doesn't take into account the plate speed and
    //acceleration
    in[0] = state->ball_position;
    in[1] = state->ball_speed;
    in[2] = state->ball_acl;
    in[3] = state->theta;
}

