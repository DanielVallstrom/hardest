// Module containing the main-function for hardest.
// hardest finds fewest questions that solve the generalization of
// "The hardest logic puzzle ever".


/*
    Copyright (C) 2026 Daniel Vallstrom. All rights reserved.

    Unless explicitly acquired and licensed from Licensor under a license
    other than the Reciprocal Public License ("RPL"), the contents of this
    file are subject to the RPL Version 1.1, or subsequent versions as
    allowed by the RPL, and You may not copy or use this file in either
    source code or executable form, except in compliance with the terms
    and conditions of the RPL.

    You should be able to find a copy of the RPL (the "License") in a file
    named LICENSE that should come along with this file; if not, write to
    daniel.vallstrom@gmail.com.

    All software distributed under the License is provided in the hope
    that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE. See the License for more details.
*/


#include "hard.h"
#include "options.h"
#include "common.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>



#if 0
// Prints various info.
static void printMoreInfo( HardInstance * hi )
{
    Settings * s = hi->settings;

    printf(
"  verbosity-vector=%#llx",
(unsigned long long int)s->verbosityVector
          );

    putc( '\n', stdout );
}
#endif



int main( int argc, char * * argv )
{
    // For measuring cpu time used.
    clock_t clockStart = clock();

    HardInstance * hi = hard_newInstance();

    if ( hi == NULL )
    {
        fprintf( stderr, "Error: not enough memory\n" );

        return 1;
    }


    // Used for various calls.
    int result;


    // Read the command line options.
    {
        result = options_parseCommandLineOptions( hi, argc, argv );

        if ( result > 0 )
        {
            return result;
        }
    }


    // Allocate arrays. And check gods.
    if ( hard_allocArrays( hi ) )
    {
        if ( hi->settings->verbosityVector & HardVerbosity_printErrors )
        {
            fprintf( stderr, "Error: wrong number of gods, or "
                             "not enough memory\n" );
        }

        return 1;
    }


    // Print various info.
    if ( hi->settings->verbosityVector & HardVerbosity_printMore )
    {
        //printMoreInfo(hi);
    }


    // Print seed
    if ( hi->settings->verbosityVector & HardVerbosity_printSeed )
    {
        printf( "seed: %lu\n", hi->settings->seed );
    }


    // Save current time.
    if ( hi->settings->verbosityVector & HardVerbosity_printTime )
    {
	      clockStart = clock();
    }


    // Solve the problem.
    if ( hard_solve(hi) == Hard_outOfMemory )
    {
        if ( hi->settings->verbosityVector & HardVerbosity_printErrors )
        {
            fprintf( stderr, "Error\n\n" );
        }

        return 1;
    }


    // Print cpu time used.
    if ( hi->settings->verbosityVector & HardVerbosity_printTime )
    {
        printf( "cpu time used, in seconds: %.3f\n\n",
                (double)(clock()-clockStart) / CLOCKS_PER_SEC );
    }


    hard_free(hi);


    return 0;
}
