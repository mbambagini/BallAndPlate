#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <reinforcement.h>



/******************************************************************************
 * VARIABLES
 ******************************************************************************/

static double ace_lambda;
static double ace_beta;
static int ace_n_inputs;
static double *ace_v;
static double *ace_act;



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

char ace_initialize ( double lambda, double beta, int n_inputs )
{
    ace_lambda = lambda;
    ace_beta = beta;
    ace_n_inputs = n_inputs;
    ace_v = malloc(sizeof(double)*n_inputs);
    ace_act = malloc(sizeof(double)*n_inputs);

    ace_clear_weights();
    ace_clear_act();

    return 1;
}

void ace_step ( int x )
{
    int i;
    for (i=0; i<ace_n_inputs; i++)
        ace_act[i] *= ace_lambda;
    if (x>-1)
        ace_act[x] += 1-ace_lambda;
}

double ace_get( int x )
{
    return ace_v[x];
}

void ace_update ( double r )
{
    int i;
    for (i=0; i<ace_n_inputs; i++)
        ace_v[i] += r * ace_beta * ace_act[i];
}


void ace_clear_act ()
{
    int i;
    for (i=0; i<ace_n_inputs; i++)
        ace_act[i] = 0.0;
}


void ace_clear_weights ()
{
    int i;
    for (i=0; i<ace_n_inputs; i++)
        ace_v[i] = 0.0;
}


void ace_serialize ( char *filename )
{
    int i;

    FILE* file = fopen(filename, "w");
    if (file==NULL)
        return;

    fprintf(file, "%d %f %f\n", ace_n_inputs, ace_beta, ace_lambda);

    for (i=0; i<ace_n_inputs; i++)
        fprintf(file, "%f %f\n", ace_v[i], ace_act[i]);

    fclose(file);
}

char ace_deserialize ( char *filename )
{
    int i;

    FILE* file = fopen(filename, "r");
    if (file==NULL)
        return 0;

    fscanf (file, "%d", &(ace_n_inputs));
    fscanf (file, "%lf", &(ace_beta));
    fscanf (file, "%lf", &(ace_lambda));

    ace_v = malloc(sizeof(double)*(ace_n_inputs));
    ace_act = malloc(sizeof(double)*(ace_n_inputs));

    for (i=0; i<ace_n_inputs; i++)
        fscanf(file, "%lf %lf", &(ace_v[i]), &(ace_act[i]));

    fclose(file);

    return 1;
}

