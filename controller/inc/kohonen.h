/*! \file kohonen.h
 *  \date 04/07/11
 *
 *  \brief Function prototypes to manage and use a Kohonen network.
 *
 * This library is able to manage only one Kohonen network
 *
 */

#ifndef __KOHONEN_H__
#define __KOHONEN_H__



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/*! \brief Initialize a Kohonen network
 *
 * \param n_inputs number of inputs for each neuron
 * \param n_rows number of rows in the neuron map
 * \param n_cols number of columns in the neuron map
 * \param max_alpha maximum alpha value
 * \param min_alpha minimum alpha value
 * \param radius initial radius (to find the neighborhood of the winning neuron)
 *
 * \return 0: error, 1: done
 */
char kohonen_init ( int n_inputs, int n_rows, int n_cols,
                    double max_alpha, double min_alpha,
                    int radius,    int iter, double lower_weight);

/*! \brief Execute a learning step
 *
 * This function finds the winner, updates its neighborhood,
 * updates the learning coefficients and returns an integer value, which
 * identifies uniquely the winner neuron
 *
 * \param input the input vector
 *
 * \return the winner neuron
 */
int kohonen_step ( double* input );

/*! \brief Return the winner neuron
 *
 * This function is similar to kohonen_step, without executing the learning
 * updatings.
 *
 * \param input the input vector
 *
 * \return the winner neuron
 */
int kohonen_get ( double* input );



/******************************************************************************
 * I/O FUNCTIONS
 ******************************************************************************/

/*! \brief Serialize the network
 *
 * \param filename string containing the destination file path
 */
void kohonen_serialize ( char* filename );

/*! \brief Deserialize the network
 *
 * \param filename string containing the source file path
 *
 * \return 0: error, 1: done
 */
char kohonen_deserialize ( char* filename );



#endif //__KOHONEN_H__

