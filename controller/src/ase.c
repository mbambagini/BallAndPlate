#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <reinforcement.h>



/******************************************************************************
 * VARIABLES
 ******************************************************************************/

static int ase_n_boxes;
static double ase_lambda;
static double ase_alpha;
static double *ase_weights;
static double *ase_eligibility;



/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

char ase_initialize ( int n_boxes, double lambda, double alpha )
{
    int i;

    //memory allocation
    ase_weights = malloc(sizeof(double)*n_boxes);
    if (ase_weights==NULL)
        return 0;
    ase_eligibility = malloc(sizeof(double)*n_boxes);
    if (ase_eligibility==NULL)
        return 0;

    //parameter settings
    ase_n_boxes = n_boxes;
    ase_lambda = lambda;
    ase_alpha = alpha;

    //memory cleaning
    for (i=0; i<n_boxes; i++) {
        ase_weights[i] = 0.0;
        ase_eligibility[i] = 0.0;
    }

    return 1;
}

void ase_clear_eligibility ()
{
    int i;
    for (i=0; i<ase_n_boxes; i++)
        ase_eligibility[i] = 0.0;
}


double ase_step ( int x )
{
    int i;
    double lambda;
    double out = 0., net;

    if (x>-1) {
        net = ase_weights[x]+0.05*(rand()%10+1);
        out = (net>=0.0)?1:-1;
    }

    lambda = ase_lambda;

    for (i=0; i<ase_n_boxes; i++)
        ase_eligibility[i] *= lambda;

    if (x>-1)
        ase_eligibility[x] += out*(1-lambda);

    return out;
}

double ase_get ( int x )
{
    double net, out;

    net = ase_weights[x]+0.05*(rand()%10+1);
    out = (net>=0.0)?1:-1;
    return out;
}

void ase_update ( double r )
{
    int i;
    for (i=0; i<ase_n_boxes; i++)
        ase_weights[i] += r*ase_alpha*ase_eligibility[i];
}

void ase_serialize ( char *filename )
{
    int i;

    FILE* file = fopen(filename, "w");

    fprintf(file, "%d %f %f\n", ase_n_boxes, ase_lambda, ase_alpha);
    
    for (i=0; i<ase_n_boxes; i++)
        fprintf(file, "%f %f\n", ase_weights[i], ase_eligibility[i]);

    fclose(file);

}

char ase_deserialize ( char *filename )
{
    int i;

    FILE* file = fopen(filename, "r");

    fscanf (file, "%d", &(ase_n_boxes));
    fscanf (file, "%lf", &(ase_lambda));
    fscanf (file, "%lf", &(ase_alpha));

    ase_weights = malloc(sizeof(double)*(ase_n_boxes));
    if (ase_weights==NULL)
        return 0;
    ase_eligibility = malloc(sizeof(double)*(ase_n_boxes));
    if (ase_eligibility==NULL)
        return 0;

    for (i=0; i<ase_n_boxes; i++)
        fscanf(file, "%lf %lf", &(ase_weights[i]), &(ase_eligibility[i]));

    fclose(file);

    return 1;
}

