//
// File: simple_pc_funcs_test.c
//

#include "simple_pc_funcs.h"

int main( int argc, char *argv[] ) {

    if( argc < 4 ) {
        printf( "  usage: ./simple_pc_funcs_test <base_val> <lower_incr> <upper_incr>\n" );
        printf( "example: for result between 50,000 and 100,000, use -\n" );
        printf( "         ./simple_pc_funcs_test 50000 0 50000\n" );
        return 1;
    }

    int base  = atoi( argv[1] );
    int lower = atoi( argv[2] );
    int upper = atoi( argv[3] );

    int compilation_input = do_rand_gen( base, lower, upper );
    printf( "Computed compilation input: %d\n", compilation_input );

    char result[128] = { 0 };
    do_compilation( compilation_input, result );
    printf( "%s\n", result );
    return 0;

} // End main()...)
