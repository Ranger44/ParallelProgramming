#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "simd.p5.h"

//Run by default with 1M array size unless script overrides ARRAYSIZE
#ifndef ARRAYSIZE
#define ARRAYSIZE   1024*1024
#endif

//Run by default 10 times to get an average performance
#ifndef NUMTIMES
#define NUMTIMES    10
#endif

int main()
{
    //Confirm OpenMP is supported by current machine
    #ifndef _OPENMP
        fprintf(stderr, "OpenMP is not supported on this machine.\n");
        return 1;
    #endif

    //Define random arrays to multiply
    float *arrayA = new float[ARRAYSIZE];
    float *arrayB = new float[ARRAYSIZE];
    float *arrayC = new float[ARRAYSIZE];

    //Populate arrayA and arrayB with square roots of index number
    for(int i = 0; i < ARRAYSIZE; i++)
        arrayA[i] = arrayB[i] = sqrt(i);

    //Store results for SimdMul
    double maxSimdMM = 0.;
    double sumSimdMM = 0.;
    double avgSimdMM = 0.;

    //Store results for non-SimdMul
    double maxNSimdMM = 0.;
    double sumNSimdMM = 0.;
    double avgNSimdMM = 0.;

    //Store results for SimdMulSum
    double maxSimdSMM = 0.;
    double sumSimdSMM = 0.;
    double avgSimdSMM = 0.;

    //Store results for non-SimdMulSum
    double maxNSimdSMM = 0.;
    double sumNSimdSMM = 0.;
    double avgNSimdSMM = 0.;

    //Loop 10 times to get average performance

    for(int times = 0; times < NUMTIMES; times++)
    {
        //Do SimdMul
        //Start timer
        double time0 = omp_get_wtime();
        //Time SimdMul
        SimdMul(arrayA, arrayB, arrayC, ARRAYSIZE);
        //Stop Timer
        double time1 = omp_get_wtime();
        //Process Current Performance
        double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
        if(megaMults>maxSimdMM)
            maxSimdMM = megaMults;
        sumSimdMM += megaMults;

        //Do non-SimdMul
        //Start timer
        time0 = omp_get_wtime();
        //Time Non-SimdMul
        for(int i = 0; i < ARRAYSIZE; i++)
            arrayC[i] = arrayA[i] * arrayB[i];
        //Stop Timer
        time1 = omp_get_wtime();
        //Process Current Performance
        megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
        if(megaMults>maxNSimdMM)
            maxNSimdMM = megaMults;
        sumNSimdMM += megaMults;

        //Do SimdMulSum
        //Start timer
        time0 = omp_get_wtime();
        //Time SimdMulSum
        float result = SimdMulSum(arrayA, arrayB, ARRAYSIZE);
        //Stop Timer
        time1 = omp_get_wtime();
        //Process Current Performance
        megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
        if(megaMults>maxSimdSMM)
            maxSimdSMM = megaMults;
        sumSimdSMM += megaMults;

        //Do non-SimdMulSum
        result = 0;
        //Start timer
        time0 = omp_get_wtime();
        //Time non-SimdMulSum
        for(int i = 0; i < ARRAYSIZE; i++)
            result += arrayA[i]*arrayB[i];
        //Stop Timer
        time1 = omp_get_wtime();
        //Process Current Performance
        megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
        if(megaMults>maxNSimdSMM)
            maxNSimdSMM = megaMults;
        sumNSimdSMM += megaMults;
    }

    //Process Averages
    avgSimdMM = sumSimdMM/NUMTIMES;
    avgNSimdMM = sumNSimdMM/NUMTIMES;
    avgSimdSMM = sumSimdSMM/NUMTIMES;
    avgNSimdSMM = sumNSimdSMM/NUMTIMES;

    //Print Results
    fprintf(stderr, "ARRAYSIZE: %d\n", ARRAYSIZE);
    fprintf(stderr, "SimdMul Max Performance: %8.2lf\n", maxSimdMM);
    fprintf(stderr, "SimdMul Avg Performance: %8.2lf\n", avgSimdMM);
    fprintf(stderr, "Non-SimdMul Max Performance: %8.2lf\n", maxNSimdMM);
    fprintf(stderr, "Non-SimdMul Avg Performance: %8.2lf\n", avgNSimdMM);
    fprintf(stderr, "SimdMulSum Max Performance: %8.2lf\n", maxSimdSMM);
    fprintf(stderr, "SimdMulSum Avg Performance: %8.2lf\n", avgSimdSMM);
    fprintf(stderr, "Non-SimdMulSum Max Performance: %8.2lf\n", maxNSimdSMM);
    fprintf(stderr, "Non-SimdMulSum Avg Performance: %8.2lf\n\n", avgNSimdSMM);


    return 0;
}