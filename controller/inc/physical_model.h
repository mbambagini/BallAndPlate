/*! \file physical_model.h
 *  \date 04/07/11
 *
 *  \brief Function prototypes to manage and simulate the physical system
 *
 */

#ifndef __PHYSICAL_MODEL_H__
#define __PHYSICAL_MODEL_H__



/******************************************************************************
 * SIMULATION PARAMETERS
 ******************************************************************************/

/*! \brief Minimum ball position (value in meters)
 */
#define BALL_LEFT_LIMIT    -0.4

/*! \brief Maximum ball position (value in meters)
 */
#define BALL_RIGHT_LIMIT   0.4

/*! \brief Minimum plate angle (-5°) (value in radiants)
 */
#define THETA_LEFT_LIMIT   (-3.14/18.0)

/*! \brief Maximum plate angle (+5°) (value in radiants)
 */
#define THETA_RIGHT_LIMIT  (3.14/18.0)

/*! \brief Minimum angle step (±1°), sensibility (value in radiants)
 */
#define THETA_SENSIBILITY  (3.14/180.0)

/*! \brief Time sensibility, simulation tick (value in seconds)
 */
#define TIME_TICK          0.02

/*! \brief Distance between the motor and the plate (value in meters)
 */
#define DISTANCE_MOTOR_PLATE    0.040

/*! \brief Ball radius (value in meters)
 */
#define BALL_RADIUS        0.013

/*! \brief Gravity acceleration (value in meters/seconds^2)
 */
#define GRAVITY_ACCELERATION    9.81

/*! \brief Max number of iterations for each simulation (dimensionless value)
 */
#define MAX_REDUCTION_ITERATIONS    1000



/******************************************************************************
 * TYPES
 ******************************************************************************/

/*! \brief System state
 */
typedef struct {
    double ball_position; /*<! Ball position */
    double ball_speed; /*<! Ball speed */
    double ball_acl; /*<! Ball acceleration */
    double theta; /*<! Plate angle */
    double omega; /*<! Plate speed */
    double omegadot; /*<! Plate acceleration */
} t_state;



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/*! \brief Set a state variable according to the given system parameters
 *
 * \param ball_position Ball position
 * \param ball_speed ball speed
 * \param ball_acl Ball acceleration
 * \param theta Plate angle
 * \param omega Plate speed
 * \param omegadot Plate acceleration
 * \param state Variable to be set
 */
void model_initialize (double ball_position, double ball_speed, double ball_acl,
                       double theta, double omega, double omegadot,
                       t_state *state);

/*! \brief Translate the state into a valid neural network input
 *
 * \param in array containing the translated state
 * \param state state to translate
 */
void model_state_copy_kn ( double *in, t_state *state );

/*! \brief Detect a failure confition
 *
 * For this function, a failure happens when the ball doesn't stay within
 * a particular plate area.
 * This area is function of the time, descreasing from an initial state
 * (equal to the whole plate surface) up to a minimum area, concentric with
 * the plate
 *
 * \param state actual state
 * \param iteration actual simulation time
 *
 * \return 0:no failure detected, 1: failure detected
 */
char model_failure ( t_state *state, unsigned long int iteration );

/*! \brief Simulate the system behavoiur
 *
 * This function, taking into account the actual state and the time elapsed from
 * the previous simulation, updates the state argument according to the new
 * system configuration
 *
 * \param state actual state of the system
 * \param delta_t time elapsed from the previous simulation
 * \param newtheta the new plate angle
 */
void model_simulate (t_state *state, double delta_t, double newtheta, double F);



#endif //__PHYSICAL_MODEL_H__

