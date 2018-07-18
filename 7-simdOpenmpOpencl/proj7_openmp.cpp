#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 32768

#ifndef NUMT
#define NUMT 4
#endif

#ifndef NUMTRIES
#define NUMTRIES 10
#endif

int main()
{
    //Setup openMP
    #ifndef _OPENMP
        fprintf(stderr, "OpenMP is not supported on this machine. \n");
        return 1;
    #endif
    omp_set_num_threads(NUMT);
    fprintf(stderr, "Now running using %d threads.\n", NUMT);

    //Array to store values from read file
    float Array[2*SIZE];
    //Array to store autocorrelation sums
    float  Sums[1*SIZE];

    int Size;
    FILE * fp;
    int i;

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
        Array[i+Size] = Array[i];
    }
    fclose(fp);

    double maxMegaMults = 0;
    double sumMegaMults = 0;
    for(int times = 0; times < NUMTRIES; times++){
        //Autocorrelate
        double time0 = omp_get_wtime();
        #pragma omp parallel for
        for( int shift = 0; shift < Size; shift++)
        {
            float sum = 0.;
            for(int i =0; i<Size;i++)
            {
                sum+= Array[i]*Array[i+shift];
            }
            Sums[shift] = sum;
        }
        double time1 = omp_get_wtime();
        double megaMults = (double)SIZE*(double)SIZE/(time1-time0)/1000000.;
        if(maxMegaMults < megaMults)
            maxMegaMults = megaMults;
        sumMegaMults += megaMults;
    }
    double avgMegaMults = sumMegaMults/(double)NUMTRIES;

    //Display Results
    fprintf(stderr, "Max Performance: %8.2lf MegaMults/sec\n", maxMegaMults);
    fprintf(stderr, "Avg Performance: %8.2lf MegaMults/sec\n", avgMegaMults);

    /*
    for(i = 1; i<513; i++)
        fprintf(stderr,"%f\n",Sums[i]);
    */


    return 0;
}