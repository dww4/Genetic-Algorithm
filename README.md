# Genetic-Algorithm

############################### Project Title ###############################

Genetic algorithm (GA) for optimizing parameters to a function that fits data

-Written by Dejuante W. Walker 

############################### Dependencies ###############################

1. mainGA_y1.c
2. mainGA.h
3. "Generations" folder 
4. "Exercise2.txt" <- data file in the format [x,y1,y2]

############################### Executing Program ###############################

1. Make sure all dependencies are in same directory

2. Create the executable:
	a. Run: 
		clang mainGA_y1.c

3. Run the genetic algorithm:

	a. Run: 
		./a.out -n 100 -a 2 -p 10 -m 100 -l 0.5 -r 1 -k 5
	
	b. Parameters: population size(-n) # of parameters to fit(-a) # of parents(-p) maximum # of generations(-m) interpolation parameter(-l) mutation rate(-r) truncation size (-k)

	c. Should output:
		i. Generations/npop... (Estimated pxarameters at each generation)

############################### Help ###############################

Ask DJ (dejuante1503@gmail.com) 
