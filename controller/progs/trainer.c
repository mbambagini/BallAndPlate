#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>



#include <kohonen.h>
#include <physical_model.h>
#include <reinforcement.h>


#define VERSION "1.0"

#define PRINT_PERIOD 10
#define MAX_TRAINING_DURATION 1000000L


extern int system_learning (unsigned long int max_duration);


char stop = 0;

void signal_handler (int signal)
{
    printf("signal %d captured\n", signal);
    stop = 1;
}


int main (int argc, char* argv[])
{
	unsigned long int iterations = 0, best_duration = 0, partial_best = 0;
	unsigned long int partial_worst = 1000000L, counter = 0, summer = 0;
	char *output_filename = NULL;
    char *kohonen_filename = NULL;
    char *ace_filename = NULL;
    char *ase_filename = NULL;
    char run_flag = 0;
    struct stat statbuf;
    FILE* file_output;

    //Initialization
    signal(SIGINT, signal_handler);
	srand ( time(0) );

    //Parameter parsing
    while (1) {
        char c = getopt (argc, argv, "vk:c:s:o:r");
        if (c==-1)
            break;
        switch(c) {
        case 'v':
            printf("Version: %s\n", VERSION);
            exit(0);
        case 'k':
            kohonen_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(kohonen_filename, optarg);
            break;
        case 'c':
            ace_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(ace_filename, optarg);
            break;
        case 's':
            ase_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(ase_filename, optarg);
            break;
        case 'o':
            output_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(output_filename, optarg);
            break;
        case 'r':
            run_flag = 1;
            break;
        default:
            break;
        };
    }

    if (run_flag==0) {
        printf("\\***** BALL and PLATE training program *****\\\n");
        printf("version: %s\n\n", VERSION);
        printf("Info options:\n");
        printf("-r           :\trun the simulation\n");
        printf("-k [filename]:\tKohonen network to use for input and output\n");
        printf("-s [filename]:\tASE network to use for input and output\n");
        printf("-c [filename]:\tACE network to use for input and output\n");
        printf("-o [filename]:\ttraining performances\n");
        printf("\n If you are working under Cygwin, please make sure to provide double\n");
        printf(" backslashs in the networks path, for instance 'networks//kohonen.nn'.\n");  
        exit(0);
    }

    //Neural networks initialization
    if (kohonen_filename!=NULL && stat(kohonen_filename, &statbuf)!=-1)
        kohonen_deserialize(kohonen_filename);
    else
        kohonen_init(4,35, 20, 0.4, 0.00001, 10, MAX_TRAINING_DURATION, 0.0001);

    if (ase_filename!=NULL && stat(ase_filename, &statbuf)!=-1)
        ase_deserialize(ase_filename);
    else
        ase_initialize (700, 0.75, 10.0);

    if (ace_filename!=NULL && stat(ace_filename, &statbuf)!=-1)
        ace_deserialize(ace_filename);
    else
        ace_initialize (0.4, 0.5, 700);

    if (output_filename!=NULL)
        file_output = fopen(output_filename, "w");
    else
        file_output = NULL;

    //Training phase
    fprintf(stdout, "\n\nBeginning system test...\n\n");

    while(stop==0) {
        //Simulation
        unsigned long int tmp = system_learning (MAX_TRAINING_DURATION);

        //Output and result storing
        if (tmp>best_duration)
            best_duration = tmp;
        if (tmp>partial_best)
            partial_best = tmp;
        if (tmp<partial_worst)
            partial_worst = tmp;

        summer += tmp;
        counter ++;
        iterations++;

        if (counter>=PRINT_PERIOD) {
            printf("iterations: %10.ld,\t", iterations);
            printf("best time: %10.ld,\t", best_duration);
            printf("partial best: %10.ld,\t", partial_best);
            printf("partial worst: %10.ld,\t", partial_worst);
            printf("average: %10.2f\n", (float)summer/counter);
            if (file_output!=NULL) {
                fprintf(file_output, "%10.ld,\t", iterations);
                fprintf(file_output, "%10.ld,\t", best_duration);
                fprintf(file_output, "%10.ld,\t", partial_best);
                fprintf(file_output, "%10.ld,\t", partial_worst);
                fprintf(file_output, "%10.2f\n", (float)summer/counter);
            }
            counter = 0;
            summer = 0;
            partial_best = 0;
            partial_worst = best_duration;
        }
    }

    //Neural network serializations
    if (ace_filename!=NULL) {
        ace_serialize(ace_filename);
        printf("ACE network: serialized\n");
    }
    if (ase_filename!=NULL) {
        ase_serialize(ase_filename);
        printf("ASE network: serialized\n");
    }
    if (kohonen_filename!=NULL) {
        kohonen_serialize(kohonen_filename);
        printf("Kohonen network: serialized\n");
    }

    //Simulation termination
    if (file_output!=NULL)
        fclose(file_output);

    fprintf(stdout, "\nEnding system test...\n\n");
    return 0;
}

