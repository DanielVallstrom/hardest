// Module extracting data from csv bound file.


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


#include "readBounds.h"
#include "hard.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <ctype.h>



// Eats up a line.
static inline void eatLine( FILE * file )
{
    int c;

    do
    {
        c = getc(file);
    }
    while ( c != '\n'  &&  c != EOF );
}


// Eats until character d. Returns true iff d was not reached.
static inline bool eatUntilChar( FILE * file, int d )
{
    int c;

    do
    {
        c = getc(file);
    }
    while ( c != d  &&  c != EOF );

    return c != d;
}


// Eats until character d. Returns last read character.
static inline int eatUntilCharRet( FILE * file, int d )
{
    int c;

    do
    {
        c = getc(file);
    }
    while ( c != d  &&  c != EOF );

    return c;
}


// Eats until characters d or '\n'. Returns last read character.
static inline int eatUntilCharLF( FILE * file, int d )
{
    int c;

    do
    {
        c = getc(file);
    }
    while ( c != d  &&  c != '\n'  &&  c != EOF );

    return c;
}


// Eats until number. Returns last read character.
static inline int eatUntilDigitRet( FILE * file )
{
    int c;

    do
    {
        c = getc(file);
    }
    while ( !isdigit(c)  &&  c != EOF );

    return c;
}


// Like eatUntilChar but also prints the parsed file content.
static bool printUntilChar( FILE * file, int d, FILE * outfile )
{
    int c = getc(file);

    while ( c != d  &&  c != EOF )
    {
        putc( c, outfile );
        c = getc(file);
    }

    return c != d;
}


#if 0
// Like eatUntilCharLF but also prints the parsed file content.
static int printUntilCharLF( FILE * file, int d )
{
    int c = getc(file);

    while ( c != d  &&  c != '\n'  &&  c != EOF )
    {
        putchar(c);
        c = getc(file);
    }

    return c;
}
#endif


// Parses the first line of a csv file.
//   Returns true iff something went wrong.
//   For now, we'll just skip the header.
static bool parseHeader( HardInstance * hi )
{
    Settings * s = hi->settings;

    FILE * file = s->boundsFile;

    //uint8_t c = 1;  // The current column.

    // For now, we'll just skip the header.
    eatLine(file);

    return false;
}


// Reads a number from file 'file' using int c and integer n. The number is
// stored in n. c contains in the end the character after the number. At start
// c must be the first character digit in the number!
#define getNumber( file, c, n )            \
    (n) = (c) - '0';                       \
    (c) = getc(file);                      \
    while ( isdigit( c ) )                 \
    {                                      \
        (n) = 10 * (n) + ( (c) - '0' );    \
        (c) = getc(file);                  \
    }


// Reads real number from file, which is placed in x. Returns true iff a 
// parse error occurred. 
//   A trailing non-comma character will result in parse error. 
//   The number must not be negative and have no exponent. E.g. "55.666"
// "55." and "242" are valid strings. The string must begin with a digit so
// e.g. ".666" isn't valid.
static bool getReal( FILE * file, double * x )
{
    // Contains the accumulated number.
    double y;

    int c = getc(file);

    if ( !isdigit(c) )
    {
        return true;
    }

    y = c - '0';

    c = getc(file);
    while ( isdigit(c) )
    {
        y = 10 * y + ( c - '0' );
        c = getc(file);
    }

    if ( c == '.' )
    {
        c = getc(file);
        for ( double z = 10; isdigit(c); c = getc(file), z *= 10 )
        {
            y += ( c - '0' ) / z;
        }
    }

    if ( c != ',' )
    {
        return true;
    }
        
    *x = y;

    return false;
}



// Parses the body of the bounds csv file.
//   Returns true iff something went wrong.
static bool parseBody( HardInstance * hi )
{
    Settings * s = hi->settings;
    Hard * h = hi->hard;
    GodsN f = h->fGodsN;
    GodsN t = h->tGodsN;
    GodsN r = h->rGodsN;

    FILE * file = s->boundsFile;
    FILE * outFile = s->outFile;

    uint32_t rows = 1;  // Number of lines in csv file, including header.

    int chr;


    do
    {   // Parse one row.

        //uint8_t col = 1;  // The current column.

        uint32_t n;  // Used for reading numbers.

        // Read false column.
        //   If EOF comes early, the line will be treated as
        // white space, ending the file, and not treated as an error.

        chr = eatUntilDigitRet(file);

        if ( chr == EOF )
        {
            return false;
        }

        rows++;

        getNumber( file, chr, n );

        if ( chr != ',' )
        {
            fprintf( stderr, "Parse error: %c must follow number of false gods. "
                             "It was %c on line %u in csv file.\n\n",
                             ',', chr, rows );
            return true;
        }

        if ( n != f )
        {
            // Skip row.
            eatLine(file);

            continue;
        }
    

        // Read true column.

        chr = eatUntilDigitRet(file);

        if ( chr == EOF )
        {
            fprintf( stderr, "Parse error on line %u in csv file.\n\n",
                             rows );

            return true;
        }

        getNumber( file, chr, n );

        if ( chr != ',' )
        {
            fprintf( stderr, "Parse error: %c must follow number of true gods. "
                             "It was %c on line %u in csv file.\n\n",
                             ',', chr, rows );
            return true;
        }

        if ( n != t )
        {
            // Skip line.
            eatLine(file);

            continue;
        }


        // Read random column.

        chr = eatUntilDigitRet(file);

        if ( chr == EOF )
        {
            fprintf( stderr, "Parse error on line %u in csv file.\n\n",
                             rows );

            return true;
        }

        getNumber( file, chr, n );

        if ( chr != ',' )
        {
            fprintf( stderr, "Parse error: %c must follow number of random gods. "
                             "It was %c on line %u in csv file.\n\n",
                             ',', chr, rows );
            return true;
        }

        if ( n != r )
        {
            // Skip line.
            eatLine(file);

            continue;
        }


        // This is the row for the instance.

        // Read upper bound.
        double bound;
        if ( getReal( file, &bound ) )
        {
            fprintf( stderr, "Parse error reading bound on line %u in csv file.\n\n",
                             rows );

            return true;
        }

        s->upperBoundInFile = bound;

        // Print info.
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( outFile, "\nFrom the upper bounds file:\n"
                     "upper bound: %g\n", bound );
        }


        // This is the status column.

        chr = getc(file);

        switch (chr)
        {
        case 'o':
            s->boundStatus = HardBoundStatus_optimal;

            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( outFile, "status: optimal\n" );
            }

            break;
        
        case 'c':
            s->boundStatus = HardBoundStatus_conjectured;

            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( outFile, "status: conjectured optimal\n" );
            }

            break;

        case 'l':
            s->boundStatus = HardBoundStatus_likely;

            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( outFile, "status: likely optimal\n" );
            }

            break;

        case 'u':
            s->boundStatus = HardBoundStatus_upperBound;

            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( outFile, "status: upper bound\n" );
            }

            break;

        default:
            fprintf( stderr, "Parse error reading status on line %u in csv file.\n\n",
                             rows );
            return true;
        }

        // We'll just check first character of the status. This is a bit sloppy.
        eatUntilChar( file, ',' );


        // Here are notes.

        chr = getc(file);

        if ( chr != ',' )
        {
            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                if ( printUntilChar( file, ',', outFile ) )
                {
                    fprintf( stderr, "Parse error reading notes on line %u in csv file.\n\n",
                                     rows );

                    return true;
                }

                putc( '\n', outFile );
            }
            else
            {
                eatUntilChar( file, ',' );
            }
        }


        // Here is the column for the version.
        if ( ( s->boundStatus == HardBoundStatus_upperBound  ||
               s->boundStatus == HardBoundStatus_likely )  &&
             s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf ( outFile, "version: " );

            if ( printUntilChar( file, ',', outFile ) )
            {
                fprintf( stderr, "Parse error reading version on line %u in csv file.\n\n",
                                 rows );

                return true;
           }

           putc( '\n', outFile );
        }
        else
        {
            eatUntilChar( file, ',' );
        }


        // Now comes the reproduction command. We'll just print it.

        chr = getc(file);

        // If carriage return is used, read again. (Why use CRLF?! Ask AI agents
        // to fix their files? Actually, it seems that CRLF is in some old csv
        // standard document.)
        if ( chr == '\r' )
        {
            chr = getc(file);
        }

        // Print info.
        if ( chr != '\n'  &&  chr != EOF )
        {
            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( s->outFile, "reproduction command:\n" );
                putc ( chr, outFile );
                printUntilChar( file, '\n', outFile );
                putc( '\n', outFile );
            }
            else
            {
                eatLine(file);
            }
        }

        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            putc( '\n', outFile );
        }

        // Should we skip the rest of the file, or check for duplicates?
        //return false;
    }
    while ( chr != EOF );  // EOF is handled at the start, mostly.


    return false;
}


// Parses the bounds file. Returns true iff an error occurred.
bool readBounds_readFile( HardInstance * hi )
{
    if ( parseHeader(hi) )
    {
        return true;
    }

    if ( parseBody(hi) )
    {
        return true;
    }

    return false;
}

