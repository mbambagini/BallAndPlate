/*! \file reinforcement.h
 *  \date 04/07/11
 *
 *  \brief Function prototypes to manage and use the ACE-ASE networks.
 *
 * This library is able to manage only one ACE-ASE network
 *
 */

#ifndef __REINFORCEMENT_H__
#define __REINFORCEMENT_H__



/******************************************************************************
 * ASE FUNCTIONS
 ******************************************************************************/

/*! \brief Initialize the ASE network
 *
 * \param n_boxes number of neurons
 * \param lambda learning coefficient
 * \param alpha learning coefficient
 *
 * \return 0: error, 1: done
 */
char ase_initialize ( int n_boxes, double lambda, double alpha );

/*! \brief Execute a learning step
 * 
 * This function updates the eligibilites
 *
 * \param x identifier of the activated input (inputs[x]==1)
 * \return output of the neuron x
 */
double ase_step ( int x );

/*! \brief Update the weights
 *
 * \param r failure + ACE contribution
 */
void ase_update ( double r );

/*! \brief Return the ASE output
 *
 * return = sum_i (input[i]*weight[i]) = input[x]*weight[x]
 * because: input[i]==1 if i==x
 *          input[i]==1 if i!=x
 *
 * \param x identifier of the activated input 
 * \return output of the ASE network
 */
double ase_get ( int x );

/*! \brief Clear the eligibility values
 */
void ase_clear_eligibility ();



/******************************************************************************
 * ACE FUNCTIONS
 ******************************************************************************/

/*! \brief Initialize the ACE network
 *
 * \param n_inputs number of neurons
 * \param lambda learning coefficient
 * \param beta learning coefficient
 *
 * \return 0: error, 1: done
 */
char ace_initialize ( double lambda, double beta, int n_inputs );

/*! \brief Execute a learning step
 * 
 * This function updates the activations
 *
 * \param x identifier of the activated input (inputs[x]==1)
 * \return output of the neuron x
 */
void ace_step ( int x );

/*! \brief Return the ACE output
 *
 * return = sum_i (input[i]*weight[i]) = input[x]*weight[x]
 * because: input[i]==1 if i==x
 *          input[i]==1 if i!=x
 *
 * \param x identifier of the activated input 
 * \return output of the ACE network
 */
double ace_get ( int x );

/*! \brief Update the weights
 *
 * \param r failure + ACE contribution
 */
void ace_update ( double r );

/*! \brief Clear the activation values
 */
void ace_clear_act ();

/*! \brief Clear the weights
 */
void ace_clear_weights ();



/******************************************************************************
 * I/O FUNCTIONS
 ******************************************************************************/

/*! \brief Serialize the ACE network
 *
 * \param filename string containing the destination file path
 */
void ace_serialize ( char *filename );

/*! \brief Deserialize the ACE network
 *
 * \param filename string containing the source file path
 *
 * \return 0: error, 1: done
 */
char ace_deserialize ( char *filename );

/*! \brief Serialize the ASE network
 *
 * \param filename string containing the destination file path
 */
void ase_serialize ( char *filename );

/*! \brief Deserialize the ACE network
 *
 * \param filename string containing the source file path
 *
 * \return 0: error, 1: done
 */
char ase_deserialize ( char *filename );



#endif //__REINFORCEMENT_H__

