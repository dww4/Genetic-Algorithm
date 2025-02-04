//Headers//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <getopt.h>

//Functions//
void ParseInput(int argc, char * argv[]); //Read in Command-line Inputs
void allocate(); //Allocate memory for population array
void load(); //Loads data used in fit
void swap_float(double *xp, double *yp); //Swaps float array components for bubble sort
void swap_int(int *xp, int *yp); //Swaps integer array components for bubble sort
void bubbleSort(double arr1[], int arr2[], int n); //Sorts population from least fit to most using bubble sort algorithm
void Init_Pop(); //Initialize Population
void Eval();  //Evaluate Fitness
void Trunc_Select(); //Truncation Selection
void Interp_Cross(); //Interpolation Crossover
void Mutate(); //Mutation
void Elitism(); //Asigns best out of both children and population to new generation
void Terminate(); //Ends program if solution converges

//Variables//
int g; //generation loop iterable
int npar ; //Number of parameters in fit (i.e # of bits)
int npop ; //Number of solutions in population
int nparents ; //Number of parents chosen (nparents < npop)
int Nmax ; //Maximum number of generations
int k ; //Number of individual chosen for truncation selection
int gcount; //Global counter to check convergence
double lambda ; //Interpolation  crossover factor
double r ; //Mutation rate (1/npar)
double factor ; //Factor that keeps error sum stable and not Inf
int len_data; //actual length of data
double *fitness; //population ranked by error
double *x_data, *y_1, *y_2; //data values in Exercise2.txt
double *pop; //population of solutions as a poor man's array (pop = [par1_0,par2_0,par1_1,par2_1,...])
double *best; //best parameters from previous generation
double *parents; //parents chosen
double *children; //children created from parents
double *sum_all; //errors of children + population
double *sum_pop; //sum of the errors between data and fitted function for population
double *sum_child; //sum of the errors between data and fitted function for children
int *score; //indices of the population ranked best to worst (index is of first parameter in each individual ex: [0,2,4,...] for a two parameter individual
char *outp; //output file name
char *inp; //input file name
FILE *outputfile, *inputfile ;
long idum; // Random number generator seed
