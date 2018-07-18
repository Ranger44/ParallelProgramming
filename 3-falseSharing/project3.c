/* Project 3 - CS475 - Spring 2018
** Holly Straley
** Description: Fix false sharing with both fixes mentioned in lecture.
** Fix 1: pad the rest of the structue with a certain numbers of ints
** to see the jump in performance as more cache lines are used and eventually
** each thread will be using only its own cache line
** Fix 2: accumulate the sum in a variable that is private to each thread
** 
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

//#define NUMT	1 //number of threads
#define NUM	16//padding

void falseFix1(int NUMT) {
	printf("Fix #1\n");
	printf(" Threads: %d\n", NUMT);
	struct s {
		float value;
		int pad[NUM];	//Fix 1: vary NUM to eventually get each array on own cache line
	} Array[4];
	printf(" Padding: %d\n",NUM);
		
	omp_set_num_threads( NUMT );

	int someBigNumber = 1000000000;
		
	double time0 = omp_get_wtime( );

	#pragma omp parallel for default(none), shared(Array,someBigNumber)
	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < someBigNumber; j++ ) {
			Array[ i ].value = Array[ i ].value + 2.;
		}
	}
	
	double time1 = omp_get_wtime( );

	// print performance here:::
	float megaAdds = ((float)(someBigNumber) /
		(time1 - time0) / 1000000.);
	printf("Performance: %8.4lf MegaAdds per sec\n", megaAdds);
	
	return;
}

void falseFix2(int NUMT) {
	printf("Fix #2\n");
	printf(" Threads: %d\n", NUMT);
	
	struct s {
		float value;
	} Array[4];
	
	omp_set_num_threads(NUMT);
	
	int someBigNumber = 1000000000;
	
	double time0 = omp_get_wtime( );

	#pragma omp parallel for default(none), shared(Array,someBigNumber)
	for( int i = 0; i < 4; i++ ) {
		float tmp = Array[i].value;
		for( int j = 0; j < someBigNumber; j++ ) {
			tmp = tmp + tmp + 2;
			//Array[ i ].value = Array[ i ].value + 2.;
		}
	Array[i].value = tmp;
	}
	
	double time1 = omp_get_wtime( );

	// print performance here:::
	float megaAdds = ((float)(4 * someBigNumber) /
		(time1 - time0) / 1000000.);
	printf("Performance: %8.4lf MegaAdds per sec\n", megaAdds);
	
	return;
}

int main( int argc, char *argv[ ] ) {
	int NUMTHREADS = atoi(argv[1]);
	#ifndef _OPENMP
		fprintf( stderr, "OpenMP is not available\n" );
		return 1;
	#endif
	
	falseFix1(NUMTHREADS);
//	falseFix2(NUMTHREADS);
	return 0;
}
