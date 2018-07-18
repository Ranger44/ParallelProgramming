#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT	1	
#define ARRAYSIZE	(1024 * 1024)
#define NUMTRIES	5

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];