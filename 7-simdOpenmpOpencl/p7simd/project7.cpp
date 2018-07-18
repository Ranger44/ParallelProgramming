//Autocorrelation using SIMD
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "simd.p5.h"

#define SIZE 32768

#ifndef NUMTRIES
#define NUMTRIES 10
#endif

int main()
{
    //Setup openMP
    #ifndef _OPENMP
        fprintf(stderr, "OpenMP is not supported\n");
        return 1;
    #endif
    fprintf(stderr, "SIMD\n");

    //Array to store values from read file
    float Array[2*SIZE];
    //Array to store autocorrelation sums
    float  Sums[1*SIZE];

    FILE * fp;
    int i, Size;

    //Read from the file
    fp = fopen( "signal.txt", "r" );
    if( fp == NULL )
    {
        fprintf( stderr, "Cannot open file 'signal.txt'\n");
        exit(1);
    }
    fscanf(fp, "%d", &Size);
    Size = SIZE; // Not allowing for variable sized files in this.

    //Dump data into the Array
    for( i = 0; i < Size; i++ )
    {
        fscanf( fp, "%f", &Array[i]);
        Array[i + Size] = Array[i];
    }
    fclose(fp);

    double sumMegaMults = 0;
    for(int times = 0; times < NUMTRIES; times++){
        //Autocorrelation from project description
        double time0 = omp_get_wtime();
        for( int shift = 0; shift < Size; shift++ )
        {
            Sums[shift] = SimdMulSum( &Array[0], &Array[0+shift], Size);
        }
        double time1 = omp_get_wtime();
        double megaMults = (double)SIZE*(double)SIZE/(time1-time0)/1000000.;

        sumMegaMults += megaMults;
    }
    double avgMegaMults = sumMegaMults/(double)NUMTRIES;

    //Display Results
    fprintf(stderr, "   Avg Performance: %8.2lf MegaMults/sec\n", avgMegaMults);
	
	//Points for scatterplot
	for(i = 1; i <= 512; i++) {
        fprintf(stderr, "%d %f\n", i, Sums[i]);
	}

    return 0;
}
