/*
2 experiments - timing and array multiplication + reduction
use different array size from 1000 to 32M
graph speedup (S = SIMD performance / non-SIMD performance) as Y axis
*/

#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT	1	
#define ARRAYSIZE	(1024 * 1024)
#define NUMTRIES	5

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main() {
	#ifndef _OPENMP
		fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
		return 1;
	#endif

	omp_set_num_threads(NUMT);
	fprintf(stderr, "Using %d threads\n", NUMT);
	
	double maxMegaMults = 0.0;
	double sumMegaMults = 0.0;
	double sumTime = 0.0;
	double avgTime = 0.0;

	int t;
	int i;
	for(t = 0; t < NUMTRIES; t++){ 
		double time0 = omp_get_wtime();
		
		#pragma omp parallel for
		for(i = 0; i < ARRAYSIZE; i++) {
			C[i] = A[i] * B[i];
		}
		double time1 = omp_get_wtime();
		double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
		sumTime += (time1 - time0);
		sumMegaMults += megaMults;
		if(megaMults > maxMegaMults) {
			maxMegaMults = megaMults;
		}
	}
	
	double avgMegaMults = sumMegaMults / (double) NUMTRIES;
	avgTime = sumTime / (double) NUMTRIES;
	printf("	Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
	printf("	Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);
	printf("	Average time = %.8lf millisecs\n", avgTime* 1000 );	

	return 0;
}


