/*
You are creating a month-by-month simulation of a grain-growing operation. 
The amount the grain grows is affected by the temperature, amount of precipitation, 
and the number of "graindeer" around to eat it. The number of graindeer depends on 
the amount of grain available to eat.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

float Ranf( unsigned int *seedp,  float low, float high );
int Ranf( unsigned int *seedp, int ilow, int ihigh );
void GrainDeer();
void Grain();
void Watcher();
void DeerTick();

float
SQR( float x )
{
        return x*x;
}

//state of the system
int	NowYear;		// 2017 - 2022
int	NowMonth;		// 0 - 11
int NowMonthNum;
int NowNumDT;

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int		NowNumDeer;		// number of deer in the current population

const float GRAIN_GROWS_PER_MONTH =			8.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;
const float DEER_TICKS_BORN_PER_MONTH = 		10;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =				2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

int main() {
	// starting date and time:
	NowMonth =    0;
	NowYear  = 2017;
	NowMonthNum = 0;
	
	// starting state (feel free to change this if you want):
	NowNumDeer = 10;
	NowHeight =  1.;
	NowNumDT = 40;
	
	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	unsigned int seed = 0;
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		NowPrecip = 0.;
		

	//parallel sections
	omp_set_num_threads( 4 );	// same as # of sections
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			DeerTick( );	// your own
		}
	}       // implied barrier -- all functions must return in order to allow any of them to get past here
		
	return 0;
}

float Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

void GrainDeer() {
	//deer population dependent on grain height
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tmpGrainHeight = NowHeight;
		int tmpNumDeer = NowNumDeer;
		//factor in carrying capacity to deer population
		if(tmpNumDeer > tmpGrainHeight) {
			tmpNumDeer--;
		}
		else if (tmpNumDeer < tmpGrainHeight) {
			tmpNumDeer++;
		}
		
		if(tmpNumDeer < 0) {
			tmpNumDeer = 0;
		}
		
		// DoneComputing barrier:
		#pragma omp barrier
		NowNumDeer = tmpNumDeer;
		//GrainGrowth and GrainDeer copy into global variables
		// DoneAssigning barrier:
		#pragma omp barrier
	
		// Watcher thread prints, GrainDeer() waits for that to complete - does nothing else
		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Grain() {
	//Structure of each simulation function
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tmpGrainHeight = NowHeight;
		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
		
		tmpGrainHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		tmpGrainHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		
		if(tmpGrainHeight < 0.0) {
			tmpGrainHeight = 0;
		}
		// DoneComputing barrier:
		#pragma omp barrier
		NowHeight = tmpGrainHeight;
		
		// DoneAssigning barrier:
		#pragma omp barrier
		
 		// Watcher thread prints, Grain() waits for that to complete - does nothing else
		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Watcher() {
	
while( NowYear < 2023 )
	{
		//GrainGrowth and GrainDeer compute into tmp variables
		// DoneComputing barrier:
		#pragma omp barrier
		//GrainGrowth and GrainDeer copy into global variables
		// DoneAssigning barrier:
		#pragma omp barrier
		//watcher thread prints
		float cmNowHeight = (NowHeight) * 2.54;
		float celsNowTemp = (5./9.) * (NowTemp - 32); 
		
		printf("%d\t%6.2f\t%6.2f\t%6.2f\t%d\t%d\n", NowMonthNum + 1, NowPrecip, celsNowTemp, cmNowHeight, NowNumDeer, NowNumDT);

		// Increment Month and Year
		if (NowMonth == 11) {
			NowMonth = 0;
			NowYear++;
		}
		else {
			NowMonth++;
		}
		
		NowMonthNum++;
		
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		unsigned int seed = 0;
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;
		
		// DonePrinting and Incrementing barrier:
		#pragma omp barrier
	}
}

void DeerTick() {
	while( NowYear < 2023 ) {
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int tmpNumDT = NowNumDT;
		
		if(NowNumDT / NowNumDeer < 5) {
			tmpNumDT += DEER_TICKS_BORN_PER_MONTH;
		}
		else{
			tmpNumDT -= 15;
		}
		
		if(tmpNumDT < 0) {
			tmpNumDT = 0;
		}
		
		// DoneComputing barrier:
		#pragma omp barrier
		
		NowNumDT = tmpNumDT;
		// DoneAssigning barrier:
		#pragma omp barrier

		// Watcher thread prints, DeerTick() waits for that to complete - does nothing else
		#pragma omp barrier
	}
}