//
// File: simple_pc_funcs.h
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static long delta = 0;
static long delta_max = 1000L;

int do_rand_gen( int base, int lower_incr, int upper_incr );
char * do_compilation( unsigned int number, char result[] );

// Generate a random number between (base+lower_incr) and
// (base+upper_incr).
//
// Example - this call - 
//
//    do_rand_gen( 5, 5, 10)
//
// will produce results of [10..15].
//
int do_rand_gen( int base, int lower_incr, int upper_incr )
{
#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Args: %d, %d, %d\n", base, lower_incr, upper_incr );
#endif
 
    if( ! (upper_incr > lower_incr) ) return base;

#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Random number range: 0 to %d\n", RAND_MAX );
#endif
 
    long tm_now = time(0);
    long seed = tm_now + delta++;
    if( delta > delta_max ) delta = 0;
    
    srand( seed );
    int rval = rand();
 
#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Random value: %d\n", rval );
#endif

    float ratio = (float)rval / (float)RAND_MAX;

#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Computed ratio: %f\n", ratio );
#endif

    float fincr = ( (float)( upper_incr - lower_incr) + 0.5 );

#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Computed fincr: %f\n", fincr );
#endif

    int rand_incr = ( (int)( fincr * ratio ) ) + lower_incr;

#ifdef DEBUG_PC_FUNCS_DO_RAND_GEN
    printf( "Computed incr: %d\n", rand_incr );
#endif

    return (base + rand_incr);
}

// Sum the total number of times the set of all positive integers
// up to K can each be evenly divided by any other integer.
char * do_compilation( unsigned int number, char result[] )
{

    unsigned int working; 

    unsigned int even_divisibles = 0;
    unsigned int sum_of_even_divisibles = 0;

    unsigned int J, K, K_plus;

    for( K = 1; K < number; K ++ ) {
        
        K_plus = K+1;
        even_divisibles = 0;
    
        for( J = 1; J < K_plus; J++ ) {
            working = K / J;
            if( ( working * J ) == K ) {
                even_divisibles++;
            }
        }

#ifdef DEBUG_PC_FUNCS_DO_COMPILATION
        printf( "Finding: The constant %d is evenly divided by %d different values.\n",
                (J-1), even_divisibles );
#endif

        sum_of_even_divisibles += even_divisibles;
    }

    sprintf( result, "Summary: The set of all positive integers up thru constant K of %d together"
             " can each be evenly divided by %d different values.",
            (number-1), sum_of_even_divisibles );

    return 0;
}
