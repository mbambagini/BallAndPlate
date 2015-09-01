#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <kohonen.h>



/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define inf(c, r)    (c - r >= 0 ? c - r : 0)
#define sup(c, r, m)    (c + r < m ? c + r : m - 1)



/******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct {
    int row;
    int col;
} t_kohonen_coordinate;



/******************************************************************************
 * VARIABLES
 ******************************************************************************/

//network structure
static int kn_n_inputs; /*<! Number of inputs for each neuron*/
static int kn_n_rows; /*<! Number of rows in the neuron matrix*/
static int kn_n_cols; /*<! Number of columns in the neuron matrix*/
static double ***kn_weights; /*<! neuron weights */

//neighborhood radius
static double kn_lambda;
static double kn_double_radius;
static int kn_max_radius;
static int kn_radius; //neighborhood radius

//learning ratios
static double kn_gamma;
static double kn_max_alpha;
static double kn_min_alpha;
static double kn_alpha;



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

char kohonen_init(
    int n_inputs,
    int n_rows,
    int n_cols,
    double max_alpha,
    double min_alpha,
    int radius,
    int iter,
    double lower_weight
)
{
    int i, j, k;

    //memory allocation
    kn_weights = malloc(sizeof(double**)*n_rows);
    if (kn_weights==NULL)
        return 0;
    for (i=0; i<n_rows; i++) {
        kn_weights[i] = malloc(sizeof(double*)*n_cols);
        if (kn_weights[i]==NULL)
            return 0;
        for (j=0; j<n_cols; j++) {
            kn_weights[i][j] = malloc(sizeof(double)*n_inputs);
            if (kn_weights[i][j]==NULL)
                return 0;
        }
    }

    //coefficient settings
    kn_n_inputs = n_inputs;
    kn_n_cols = n_cols;
    kn_n_rows = n_rows;

    kn_lambda = pow (1.0/radius, 2.0/iter);
    kn_max_radius = radius;
    kn_double_radius = radius;
    kn_radius = radius;

    kn_gamma = pow(min_alpha/max_alpha, 1.0/iter);
    kn_max_alpha = max_alpha;
    kn_min_alpha = min_alpha;
    kn_alpha = max_alpha;
    
    //random initialization of the weights
    for (i=0; i<n_rows; i++)
       for (j=0; j<n_cols; j++)
           for (k=0; k<n_inputs; k++)
               kn_weights[i][j][k] = lower_weight*(rand()%100+1);

    return 1;
}

double kohonen_distance( t_kohonen_coordinate coord, double *input )
{
    int k;
    double distance = 0.0;
    double tmp;
    
    for (k=0; k<kn_n_inputs; k++) {
        tmp = kn_weights[coord.row][coord.col][k]-input[k];
        distance += tmp*tmp;
    }

    return distance;
}

t_kohonen_coordinate kohonen_winner ( double *input )
{
    int i, j;
    t_kohonen_coordinate coord, tmp_coord;
    double tmp, min;

    coord.row = 0;
    coord.col = 0;
    min = kohonen_distance(coord, input);

    for (i=0; i<kn_n_rows; i++)
        for (j=0; j<kn_n_cols; j++) {
            tmp_coord.row = i;
            tmp_coord.col = j;
            tmp = kohonen_distance(tmp_coord, input);
            if (tmp <= min) {
                min = tmp;
                coord = tmp_coord;
            }
        }

    return coord;
}

void kohonen_update_neighborhood (t_kohonen_coordinate winner, double *input )
{
    int i, j, k;
    double theta, rr, dd;

    rr = 2*kn_radius*kn_radius;

    i=inf(winner.row, kn_radius);
    for (;i<=sup(winner.row, kn_radius, kn_n_rows); i++){
        j=inf(winner.col, kn_radius);
        for (;j<=sup(winner.col, kn_radius, kn_n_cols); j++) {
            dd = (i-winner.row)*(i-winner.row) + (j-winner.col)*(j-winner.col);
            dd = sqrt(dd);
            theta = 1.0-dd/rr;

            for (k=0; k<kn_n_inputs; k++) {
                double tmp;
                tmp = theta*kn_alpha*(input[k]-kn_weights[i][j][k]);
                kn_weights[i][j][k] += tmp;

            }
        }
    }
}

int kohonen_step ( double *input )
{
    t_kohonen_coordinate winner;
    
    winner = kohonen_winner (input);
    kohonen_update_neighborhood(winner, input);

//    if (k_n->coeff_alpha>k_n->coeff_min_alpha)
    kn_alpha *= kn_gamma;
//    else
//        k_n->coeff_alpha = k_n->coeff_min_alpha;
    kn_double_radius *= kn_lambda;
    kn_radius = kn_double_radius;
    kn_radius = kn_radius + 1;

    return winner.row*kn_n_cols+winner.col;
}

int kohonen_get ( double *input )
{
    t_kohonen_coordinate winner;
    
    winner = kohonen_winner (input);
    return winner.row*kn_n_cols+winner.col;
}

void kohonen_serialize ( char* filename )
{
    int i, j, k;

    FILE* file = fopen(filename, "w");
      if (file==NULL)
        return;

    //structure
    fprintf(file, "%d %d %d\n", kn_n_inputs, kn_n_rows, kn_n_cols);
    //coefficients
    fprintf(file, "%f %f %d %d\n",
                         kn_lambda, kn_double_radius, kn_max_radius, kn_radius);
    fprintf(file, "%f %f %f %f\n",
                               kn_gamma, kn_max_alpha, kn_min_alpha,  kn_alpha);
    //weights
    for(i=0; i<kn_n_rows; i++) {
        for (j=0; j<kn_n_cols; j++) {
            for (k=0; k<kn_n_inputs; k++)
                fprintf(file, "%f ", kn_weights[i][j][k]);
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

char kohonen_deserialize ( char* filename )
{
    int i, j, k;

    FILE* file = fopen(filename, "r");

    if (file==NULL)
        return 0;

    //structure
    fscanf(file, "%d", &(kn_n_inputs));
    fscanf(file, "%d", &(kn_n_rows));
    fscanf(file, "%d", &(kn_n_cols));

    kn_weights = malloc(sizeof(double**)*kn_n_rows);
    for (i=0; i<kn_n_rows; i++) {
        kn_weights[i] = malloc(sizeof(double*)*kn_n_cols);
        for (j=0; j<kn_n_cols; j++)
            kn_weights[i][j] = malloc(sizeof(double)*kn_n_inputs);
    }

    //coefficients
    fscanf(file, "%lf %lf %d %d",
             &(kn_lambda), &(kn_double_radius), &(kn_max_radius), &(kn_radius));
    fscanf(file, "%lf %lf %lf %lf",
                   &(kn_gamma), &(kn_max_alpha), &(kn_min_alpha),  &(kn_alpha));

    //weights
    for(i=0; i<kn_n_rows; i++) {
        for (j=0; j<kn_n_cols; j++) {
            for (k=0; k<kn_n_inputs; k++)
                fscanf(file, "%lf", &(kn_weights[i][j][k]));
        }
    }

    fclose(file);
    return 1;
}

