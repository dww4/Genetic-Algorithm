#include "mainGA.h"

int main(int argc, char * argv[]){
  //Read in Input Parameters
  ParseInput(argc,argv);

  //Allocate Memory//
  allocate();

  //Load Data//
  load();
  //Initialization
  Init_Pop();
  for(g=0;g<Nmax;g++){
    //printf("Parameters: %d %d %d %lf %lf\n",npop,Nmax,k,lambda,r);
    //Evaluate + Print Best Parent//
    Eval();

    //Truncation Selection//
    Trunc_Select();

    //Interpolation Crossover
    Interp_Cross();

    //Mutation
    Mutate();

    //Elitism
    //int i;
    // for(i=0;i<npar*npop;i++){
    //   children[i] = pop[i];
    //   //printf("before elitism children: %lf\n",children[i]);
    // }
    Elitism();

    //Termination
    //printf("best in pop: %lf %lf\n",pop[0],pop[1]);
    Terminate();


  }
  return 0;
}
void ParseInput(int argc, char * argv[]){

  //Define Default User Inputs//
  Nmax = 1000 ; //Maximum number of generations
  npop = 50; //Number of solutions in population
  npar = 2; //Number of parameters in fit (i.e # of bits)
  nparents = 10; //Number of parents chosen in selection (nparents < npop)
  k = 3; //Parents chosen out of top k individuals
  lambda = 0.5 ; //Interpolation  crossover factor
  r = 0.7 ; //Mutation rate (1/npar)

  int option = 0;

  if(argc < 6) printf("Warning: Using defaults for unspecified command line arguments.\n");
  while((option = getopt(argc, argv, "n:a:p:m:l:r:k:")) != -1){
      switch(option){
          // Concentration normalized by c*
          case 'n':
              sscanf(optarg, "%d", &npop);
              break;
          case 'a':
              sscanf(optarg, "%d", &npar);
              break;
          case 'p':
              sscanf(optarg, "%d", &nparents);
              break;
          case 'm':
              sscanf(optarg, "%d", &Nmax);
              break;
          case 'l':
              sscanf(optarg, "%lf", &lambda);
              break;
          case 'r':
              sscanf(optarg, "%lf", &r);
              break;
          case 'k':
              sscanf(optarg, "%d", &k);
              break;
          case '?':
              printf("Unknown option -%c.\n Execution abort.", optopt);
              exit(EXIT_FAILURE);
      }
  }
}
void allocate(){
  //Allocate Memory//
  pop = calloc(npop*npar, sizeof(double));
  parents = calloc(nparents*npar, sizeof(double));
  children = calloc(npop*npar, sizeof(double));
  sum_pop = calloc(npop,sizeof(double));
  sum_child = calloc(npop,sizeof(double));
  best = calloc(npar,sizeof(double));
  sum_all = calloc(2*npop,sizeof(double)) ;
  x_data = calloc(20000, sizeof(double)); //make number large enough to not seg fault
  y_1 = calloc(20000, sizeof(double)); //make number large enough to not seg fault
  y_2 = calloc(20000, sizeof(double)); //make number large enough to not seg fault
  score = calloc(npop,sizeof(int));
  outp = malloc(sizeof(char)*100);
  inp = malloc(sizeof(char)*100);
  gcount = 0;
  //Create File Names//
  sprintf(outp,"Generations/npop_%d_npar_%d_Nmax_%d_nparents_%d_k_%d_l_%.3lf_r_%.3lf.csv", npop,npar,Nmax,nparents,k,lambda,r);
  sprintf(inp,"Exercise2.txt");

  //Write Output File//
  outputfile = fopen(outp,"w");
  if(npar==2){
    fprintf(outputfile,"Generation,a,b,Error\n");
  }
  else if(npar==3){
    fprintf(outputfile,"Generation,a,b,c,Error\n");
  }
  fclose(outputfile);

}
void load(){
  int i;
  //Check if input data file exists//
  inputfile = fopen(inp,"r");
  if(!inputfile){
    printf("Error - could not find data file %s, exiting\n",inp);
    exit(1);
  }

  //Load in input data//
  int count = 0;
  while(!feof(inputfile)){
    fscanf(inputfile,"%lf,%lf,%lf",&x_data[count],&y_1[count],&y_2[count]);
    count++;
  }
  fclose(inputfile);

  //Save the length of data and print//
  len_data = count;
  // for(i=0;i<len_data;i++){
  //   printf("%f,%f,%f\n",x_data[i],y_1[i],y_2[i]);
  // }
}
void Init_Pop(){
  int i,j,sign;
  //Initialize Random Seed//
  idum = time(NULL) ;
  //idum = 1700492329 ;
  //printf("Warning - Seed is Fixed!\n");
  printf("SEED: %ld\n",idum);
  srand(idum); // Seed for random number generation

  //Create Random Population//
  for(i=0;i<npop;i++){
    for(j=0;j<npar;j++){
      sign = rand() % 2 ;
      if(sign==0){
        pop[i*npar + j] = (double) rand()/RAND_MAX;
      }
      else if(sign==1){
        pop[i*npar + j] = (double) -rand()/RAND_MAX;
      }
      //printf("Initial Pop: %f\n",pop[i*npar + j]);
    }
  }
}
void Eval(){
  double y,x,a,b;
  int i,j,count;
  factor = 10000; //factor to keep sum stable
  double *sum = calloc(npop,sizeof(double));
  //Track Index of Error//
  for(i=0;i<npop;i++){
    score[i] = npar*i; //index is of first parameter in each individual ex: [0,2,4,...] for a two parameter individual
  }

  //Calculate Error for Each Individual//
  //printf("Generation: %d\n",g);
  count=0;
  for(j=0;j<npop*npar;j+=npar){
    sum[count] = 0;
    for(i=0;i<len_data;i++){
      //sum[count] += (y_2[i] - pow(x_data[i],pop[j])*sin(pop[j+1]*x_data[i]+pop[j+2]))*(y_2[i] - pow(x_data[i],pop[j])*sin(pop[j+1]*x_data[i]+pop[j+2]))/factor; // |y_data - y(x)| where y(x) = a*exp(b*x)

      sum[count] += fabs((y_2[i] - pow(x_data[i],pop[j])*sin(pop[j+1]*x_data[i]+pop[j+2])))/factor; // |y_data - y(x)| where y(x) = a*exp(b*x)
    }
    //printf("a1: %lf b1: %lf c1: %lf sum1: %lf\n",pop[j],pop[j+1],pop[j+2],sum[count]);
    count++;
  }

  //Store the Errors//
  for(i=0;i<npop;i++){
    sum_pop[i] = sum[i] ;
    //printf("sum_pop: %lf\n",sum_pop[i]);
  }

  //Sort Population From Least Error to Most//
  // for(i=0;i<npop;i++){
  //   //printf("score_before: %d\n",score[i]);
  //   //printf("sum_before: %f\n",sum_pop[i]);
  // }

  bubbleSort(sum,score,npop);

  // for(i=0;i<npop;i++){
  //   printf("score_after: %d\n",score[i]);
  //   //printf("sum_after: %f\n",sum_pop[i]);
  // }

  //Store Best Parameters//
  for(i=0;i<npar;i++){
    best[i] = pop[score[0] + i] ;
    //printf("best: %lf\n",best[i]);
  }

  //Output Best Parameters//
  outputfile = fopen(outp,"a");
  fprintf(outputfile,"%d",g);
  for(i=0;i<npar;i++){
    fprintf(outputfile,",%f",pop[score[0]+i]);
  }
  fprintf(outputfile,",%lf\n",sum_pop[(int)score[0]/npar]);
  fclose(outputfile);



}
void swap_float(double *xp, double *yp) {
    double temp = *xp;
    *xp = *yp;
    *yp = temp;
}
void swap_int(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
void bubbleSort(double arr1[], int arr2[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr1[j] > arr1[j + 1]) {
              //printf("arr: %lf\n &arr: %p\n",arr[j],&arr[j]);
              swap_float(&arr1[j], &arr1[j + 1]);
              swap_int(&arr2[j], &arr2[j + 1]);
            }
        }
    }
}
void Trunc_Select(){
  int i,j,ran1;
  int count=0;
  for(i=0;i<nparents;i++){
    //printf("count: %d\n",count);
    ran1 = rand() % k ;
    //printf("ran1: %d\n",ran1);
    for(j=0;j<npar;j++){
      parents[count+j] = pop[score[ran1]+j] ;
    }
    count+=npar;
  }

  // for(i=0;i<nparents*npar;i++){
  //   printf("parents: %lf\n",parents[i]);
  // }

}
void Interp_Cross(){
  int i,j,parent1,parent2;
  for(i=0;i<npop*npar;i+=2){
    parent1 = rand() % nparents ;
    parent2 = rand() % nparents ;
    //printf("parent1: %d parent2: %d\n",parent1,parent2);

    for(j=0;j<npar;j++){
      children[i+j] = (1-lambda)*parents[2*parent1+j] + lambda*parents[2*parent2+j] ;
    }
  }

  // for(i=0;i<npar*npop;i++){
  //   printf("children_chosen: %lf\n",children[i]);
  // }
}
void Mutate(){
  int i,mut,sign;
  double dice_roll,ran1,ran2;
  //double dice = rand()/RAND_MAX;
  for(i=0;i<npop*npar;i++){
    //Randomly decide if going to mutate parameter based on mutation rate r
    dice_roll = (double) rand()/RAND_MAX;
    //printf("dice: %f\n",dice_roll);

    //Mutate Parameter
    if(dice_roll <= r){
      sign = rand() % 2 ;
      //printf("sign: %d\n",sign);
      if(sign==0){
        ran1 = (double) children[i]*rand()/RAND_MAX ;  //CRUCIAL TO FINE TUNE TO AVOID LOCAL MINS
        children[i] += 10*ran1 ;
      }
      else if(sign==1){
        ran2 = (double) children[i]*rand()/RAND_MAX ; //CRUCIAL TO FINE TUNE TO AVOID LOCAL MINS
        children[i] -= 10*ran2 ;
        // if(children[i]<0){
        //   children[i] += ran2 ; //Un-negative the number
        //   ran2 = (double) children[i]*rand()/RAND_MAX ; //CRUCIAL TO FINE TUNE TO AVOID LOCAL MINS
        //   children[i] += ran2 ; //Add A Random Number Instead
        // }
      }
    }
    //printf("children_mutant: %lf\n",children[i]);
  }


}
void Elitism(){
  int i,count1,count2,j,ind1,ind2;
  int *score_all = calloc(2*npop,sizeof(int));

  //Track Index of Error//
  for(i=0;i<2*npop;i++){
    score_all[i] = npar*i; //index is of first parameter in each individual ex: [0,2,4,...] for a two parameter individual
  }
  //Evaluate Fitness of children
  count1=0;
  for(j=0;j<npop*npar;j+=npar){
    sum_child[count1] = 0;
    for(i=0;i<len_data;i++){
      sum_child[count1] += fabs((y_2[i] - pow(x_data[i],children[j])*sin(children[j+1]*x_data[i]+children[j+2])))/factor; // |y_data - y(x)| where y(x) = a*exp(b*x)
      //sum = fabs((y_1[0] - pop[0]*exp(pop[1]*x_data[0])));
      //printf("a: %lf b: %lf sum: %lf\n",pop[0],pop[1],sum);
    }
    //printf("a2: %lf b2: %lf c2: %lf sum2: %lf\n",children[j],children[j+1],children[j+2],sum_child[count1]);
    count1++;
  }

  //Group Children + Parent Scores Together (i.e concatenate sum arrays)
  for(i=0;i<2*npop;i++){
    if(i<npop){
      sum_all[i] = sum_child[i];
      //printf("sum_child: %lf\n",sum_child[i]);
    }
    else{
      sum_all[i] = sum_pop[i-npop];
    }
    //printf("sum_all: %lf\n",sum_all[i]);
  }

  // for(i=0;i<2*npop;i++){
  //   printf("score_before: %d\n",score_all[i]);
  // }
  //Sort Children + Population Scores from Least Error to Most
  bubbleSort(sum_all,score_all,2*npop);

  // for(i=0;i<2*npop;i++){
  //   printf("sum_all: %f\n",sum_all[i]);
  // }

  //Store New Generation in Pop//
  count2 = 0;
  for(i=0;i<npop;i++){
    ind1 = score_all[i] ;
    if(ind1<npop*npar){
      for(j=0;j<npar;j++){
        pop[count2 +j] = children[ind1 + j] ;
      }
    }
    else{
      for(j=0;j<npar;j++){
        pop[count2 +j] = pop[ind1+j-npar*npop] ;
      }
    }
    count2+=npar;
  }
  // printf("Generation: %d\n",g);
  // for(i=0;i<npar*npop;i++){
  //   printf("new_pop: %lf\n",pop[i]);
  // }


}
void Terminate(){
  int i;
  int counter=0 ;
  if(g>0){
    for(i=0;i<npar;i++){
      if(pop[i] == best[i]){
        counter++;
      }
    }

    if(counter == npar){
      gcount++;
      if(gcount==2){
        printf("Converged in %d Steps: ",g);
        for(i=0;i<npar;i++){
          printf("%lf ",best[i]);
        }
        printf("Error: %lf ",sum_all[0]);
        exit(1);
      }
    }
  }


}
