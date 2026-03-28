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


// Like eatUntilCharLF but also prints the parsed file content.
static int printUntilCharLF( FILE * file, int d, FILE * outFile )
{
    int c = getc(file);

    while ( c != d  &&  c != '\n'  &&  c != EOF )
    {
        putc( c, outFile );
        c = getc(file);
    }

    return c;
}


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
    uint32_t entries = 0;  // Number of entries for the instance.

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
            break;
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

        entries++;

        // Read upper bound.

        double bound;
        if ( getReal( file, &bound ) )
        {
            fprintf( stderr, "Parse error reading bound on line %u in csv file.\n\n",
                             rows );

            return true;
        }

        // In case there are multiple bounds in the file.
        if ( bound < s->upperBoundInFile )
        {
            s->upperBoundInFile = bound;
        }

        // Print info.
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( outFile, "\nFrom the upper bounds file:\n"
                     "upper bound: %f\n", bound );
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


        // This is the ind. rep. count column.

        chr = getc(file);

        if ( !isdigit(chr) )
        {
            fprintf( stderr, "Parse error in ind. rep. count, line %u: "
                             "%c should be a digit.\n\n",
                             rows, chr );
            return true;
        }

        getNumber( file, chr, n );

        if ( chr != ',' )
        {
            fprintf( stderr, "Parse error: %c must follow ind. rep. count. "
                             "It was %c on line %u in csv file.\n\n",
                             ',', chr, rows );
            return true;
        }

        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( outFile, "independently replicated count: %u\n", n );
        }


        // Here are notes.

        chr = getc(file);

        if ( chr != ',' )
        {
            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                putc( chr, outFile );

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

        chr = getc(file);

        if ( chr != ',' )
        {
            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf ( outFile, "version: " );
                putc( chr, outFile );

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
        }


        // Now comes the reproduction command. We'll extract any seed used.
        // Otherwise we'll just print the command.

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
            uint64_t nSeed;  // Seed can be 64 bits.

            if ( s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( s->outFile, "reproduction command:\n" );
                putc ( chr, outFile );

                chr = printUntilCharLF( file, '-', outFile );
                while ( chr != '\n'  &&  chr != EOF )
                {
                    assert( chr == '-' );
                    chr = getc(file);

                    // Check if seed.
                    if ( chr == 's' )
                    {
                        chr = getc(file);

                        if ( chr == ' ' )
                        {
                            chr = getc(file);
                        }

                        getNumber( file, chr, nSeed );
                        s->boundsFileSeed = nSeed;
                        fprintf( outFile, "-s %lu", nSeed );
                        putc( chr, outFile );
                    }
                    else if ( chr == '-' )  // Check long-option.
                    {
                        chr = getc(file);
                        if ( chr == 's' )
                        {
                            chr = getc(file);
                            if ( chr == 'e' )
                            {
                                chr = getc(file);
                                if ( chr == 'e' )
                                {
                                    chr = getc(file);
                                    if ( chr == 'd' )
                                    {
                                        chr = getc(file);
                                        if ( chr == ' ' ||  chr == '=' )
                                        {
                                            chr = getc(file);
                                            getNumber( file, chr, nSeed );
                                            s->boundsFileSeed = nSeed;
                                            fprintf( outFile, "--seed %lu", nSeed );
                                            putc( chr, outFile );
                                        }
                                        else
                                        {
                                            fprintf( outFile, "--seed%c", chr );
                                        }
                                    }
                                    else
                                    {
                                        fprintf( outFile, "--see%c", chr );
                                    }
                                }
                                else
                                {
                                    fprintf( outFile, "--se%c", chr );
                                }
                            }
                            else
                            {
                                fprintf( outFile, "--s%c", chr );
                            }
                        }
                        else
                        {
                            fprintf( outFile, "--%c", chr );
                        }
                    }
                    else
                    {
                        fprintf( outFile, "-%c", chr );
                    }

                    chr = printUntilCharLF( file, '-', outFile );
                }

                putc( '\n', outFile );
            }
            else
            {
                // Search for seed.

                chr = eatUntilCharLF( file, '-' );
                while ( chr != '\n'  &&  chr != EOF )
                {
                    assert( chr == '-' );
                    chr = getc(file);

                    // Check if seed.
                    if ( chr == 's' )
                    {
                        chr = getc(file);

                        if ( chr == ' ' )
                        {
                            chr = getc(file);
                        }

                        getNumber( file, chr, nSeed );
                        s->boundsFileSeed = nSeed;
                    }
                    else if ( chr == '-' )  // Check long-option.
                    {
                        chr = getc(file);
                        if ( chr == 's' )
                        {
                            chr = getc(file);
                            if ( chr == 'e' )
                            {
                                chr = getc(file);
                                if ( chr == 'e' )
                                {
                                    chr = getc(file);
                                    if ( chr == 'd' )
                                    {
                                        chr = getc(file);
                                        if ( chr == ' ' ||  chr == '=' )
                                        {
                                            chr = getc(file);
                                            getNumber( file, chr, nSeed );
                                            s->boundsFileSeed = nSeed;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    chr = eatUntilCharLF( file, '-' );
                }
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


    if ( entries == 0 )
    {
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( s->outFile, "\nno entry found for the problem in the bounds file\n\n" );
        }
    }
    else if ( entries > 1 )
    {
        fprintf( stderr, "Error: bounds file contains %u entries for the "
                         "problem; the smallest bound is used.\n\n", entries );
    }


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

    // Update rest of hi.
    if ( hi->settings->useBoundFromFile == 1 )
    {
        if ( hi->hard->upperBound == DBL_MAX )
        {
            hi->hard->upperBound = hi->settings->upperBoundInFile;
        }
    }
    else if ( hi->settings->useBoundFromFile == 2 )
    {
        if ( hi->settings->upperBoundInFile < hi->hard->upperBound )
        {
            hi->hard->upperBound = hi->settings->upperBoundInFile;
        }
    }

    return false;
}



// Append new csv row.
static bool append( HardInstance * hi, double bound, uint64_t seed,
                    double boundUsed )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    FILE * file = s->boundsFile;

    int argc = s->argC;
    char * * argv = s->argV;

    fprintf( file, "%u,", h->fGodsN );
    fprintf( file, "%u,", h->tGodsN );
    fprintf( file, "%u,", h->rGodsN );
    fprintf( file, "%.*f,", s->precision, bound );

    // What do we want to do here?
    if ( h->rGodsN == 1 )
    {
        fprintf( file, "optimal," );
    }
    else
    {
        fprintf( file, "upper_bound," );
    }

    // Ind. rep. count.
    fprintf( file, "0," );
    
    // Notes. Now it's empty. Do we want it to include anything? You could 
    // place an identifier for the one who found the upper bound here, perhaps?
    fprintf( file, "," );

    fprintf( file, "%s,", hardestVersion );

    // Here comes the replication command. We'll add seed and -i 0 last,
    // and maybe the bound used.

    for ( int c = 0; c != argc; c++ )
    {
        fprintf( file, "%s ", argv[c] );
    }

    fprintf( file, "-s %lu -i 0", seed );    

    // Handle bound used.
    if ( s->printBoundUsed )
    {
        if ( boundUsed > 888.8 )
        {
            fprintf( file, " -u 888.8 -M 1" );        
        }
        else
        {
            fprintf( file, " -u %.*g -M 1", DBL_DECIMAL_DIG, boundUsed );        
        }
    }

    // End with CRLF.
    fprintf( file, "\r\n" );

    return false;
}



// Updates bound in the upper bounds file. Returns true iff an error occurred.
//   seed and boundUsed should be the seed and bound used for the search that 
// found the new bound.
//   Rows will end with CRLF, following some csv definition or convention,
// apparently. 
//   Will also remove duplicates of the instance.
static bool updateBound( HardInstance * hi, double bound, uint64_t seed,
                         double boundUsed )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    int argc = s->argC;
    char * * argv = s->argV;

    // Name of the temporary bounds file. 
    // The file is to be renamed as the new bounds file.
    char tmpFileName[L_tmpnam];

    if ( tmpnam(tmpFileName) == NULL )
    {
        fprintf( stderr, "\nError: Could not get a temporary file name.\n\n" );

        return true;
    }

    // Open files.

    FILE * newFile = fopen( tmpFileName, "w");

    if ( newFile == NULL )
    {
        fprintf( stderr, "\nError: Could not open a new bounds file.\n\n" );

        return true;
    }

    s->boundsFile = fopen( s->boundsFileName, "r");

    if ( s->boundsFile == NULL )
    {
        fprintf( stderr, "\nError: Could not open bounds file.\n\n" );

        fclose(newFile);

        return true;
    }


    GodsN f = h->fGodsN;
    GodsN t = h->tGodsN;
    GodsN r = h->rGodsN;

    FILE * file = s->boundsFile;

    uint32_t rows = 1;  // Number of lines in csv file, including header.
    uint32_t entries = 0;  // Number of entries for the instance.

    int chr;


    // Print header.
    printUntilChar( file, '\n', newFile );
    putc( '\n', newFile );


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
            break;
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
            // Print row and continue.
            fprintf( newFile, "%u,", n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

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
            // Print row and continue.
            fprintf( newFile, "%u,%u,", f, n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

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
            // Print row and continue.
            fprintf( newFile, "%u,%u,%u,", f, t, n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

            continue;
        }


        // This is the row for the instance.

        // Skip the rest of the line in the old file.
        eatLine(file);

        entries++;

        // If this isn't the first entry, just skip the row, without
        // printing anything.
        if ( entries > 1 )
        {
            continue;
        }

        // Print gods.
        fprintf( newFile, "%u,%u,%u,", f, t, r );

        // Print new upper bound.
        fprintf( newFile, "%.*f,", s->precision, bound );

        // This is the status column.
        if ( h->rGodsN == 1 )
        {
            fprintf( newFile, "optimal," );
        }
        else
        {
            fprintf( newFile, "upper_bound," );
        }

        // This is the ind. rep. count.
        fprintf( newFile, "0,");

        // Here are notes. We could add "by XY" here.
        putc( ',', newFile );

        fprintf( newFile, "%s,", hardestVersion );

        // Here comes the replication command. We'll add seed and -i 0 last,
        // and maybe the bound used.

        for ( int c = 0; c != argc; c++ )
        {
            fprintf( newFile, "%s ", argv[c] );
        }

        fprintf( newFile, "-s %lu -i 0", seed );    

        // Handle bound used.
        if ( s->printBoundUsed )
        {
            if ( boundUsed > 888.8 )
            {
                fprintf( newFile, " -u 888.8 -M 1" );        
            }
            else
            {
                fprintf( newFile, " -u %.*g -M 1", DBL_DECIMAL_DIG, boundUsed );        
            }
        }

        // End with CRLF.
        fprintf( newFile, "\r\n" );

    }
    while ( chr != EOF );  // EOF is handled at the start, mostly.


    fclose(file);
    fclose(newFile);

    // Update backup file.

    remove( s->backupBoundsFileName );

    if ( rename( s->boundsFileName, s->backupBoundsFileName ) )
    {
        fprintf( stderr, "error: couldn't rename backup bounds file\n\n" );
    }

    remove( s->boundsFileName );  // In case above failed, and we are on Windows.

    if ( rename( tmpFileName, s->boundsFileName ) )
    {
        fprintf( stderr, "error: couldn't rename temporary bounds file\n\n" );

        return true;
    }


    return false;
}



// Writes bound to the upper bounds file. Returns true iff an error occurred.
//   seed and boundUsed should be the seed and bound used for the search that 
// found the new bound.
//   Also updates hi.
//   Rows will end with CRLF, following some csv definition or convention,
// apparently. 
//   Uses tmpnam, which is deprecated. However, there is no standard C
// alternative.
bool readBounds_write( HardInstance * hi, double bound, uint64_t seed,
                       double boundUsed )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    // If bound isn't in the bounds file, we'll append it last. Otherwise
    // we'll replace the current row.

    if ( s->boundStatus == HardBoundStatus_undefined )
    {
        // Open the bounds file.
        s->boundsFile = fopen( hi->settings->boundsFileName, "a" );

        if ( hi->settings->boundsFile == NULL )
        {
            fprintf( stderr, "\nError: Could not open best_known_bounds.csv\n\n" );

            return true;
        }

        // Append new csv row.
        if ( append( hi, bound, seed, boundUsed ) )
        {
            fprintf( stderr, "\nError: Could not append best_known_bounds.csv\n\n" );

            fclose(s->boundsFile);

            return true;
        }

        fclose(s->boundsFile);
    }
    else
    {
        if ( updateBound( hi, bound, seed, boundUsed ) )
        {
            fprintf( stderr, "\nError: Could not update best_known_bounds.csv\n\n" );

            return true;
        }
    }


    // Update s with new values.

    if ( h->rGodsN == 1 )
    {
        s->boundStatus = HardBoundStatus_optimal;
    }
    else
    {
        s->boundStatus = HardBoundStatus_upperBound;
    }

    s->upperBoundInFile = bound;


    // Print info.
    if ( s->verbosityVector & HardVerbosity_printInfo )
    {
        fprintf( s->outFile, "Updated bounds file with new bound: %f\n\n", bound );
    }


    return false;
}



// Increments the replication counter in the upper bounds file. Returns true 
// iff an error occurred.
//   seed and boundUsed should be the seed and bound used for the search that 
// found the new bound.
//   Also replaces the replication command, if s->noteRep > 1.
//   Rows will end with CRLF, following some csv definition or convention,
// apparently. 
//   Uses tmpnam, which is deprecated. However, there is no standard C
// alternative.
bool readBounds_noteRep( HardInstance * hi, uint64_t seed, double boundUsed )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    int argc = s->argC;
    char * * argv = s->argV;

    // Name of the temporary bounds file. 
    // The file is to be renamed as the new bounds file.
    char tmpFileName[L_tmpnam];

    if ( tmpnam(tmpFileName) == NULL )
    {
        fprintf( stderr, "\nError: Could not get a temporary file name.\n\n" );

        return true;
    }

    // Open files.

    FILE * newFile = fopen( tmpFileName, "w");

    if ( newFile == NULL )
    {
        fprintf( stderr, "\nError: Could not open a new bounds file.\n\n" );

        return true;
    }

    s->boundsFile = fopen( s->boundsFileName, "r");

    if ( s->boundsFile == NULL )
    {
        fprintf( stderr, "\nError: Could not open bounds file.\n\n" );

        remove(tmpFileName);
        fclose(newFile);

        return true;
    }


    GodsN f = h->fGodsN;
    GodsN t = h->tGodsN;
    GodsN r = h->rGodsN;

    FILE * file = s->boundsFile;

    uint32_t rows = 1;  // Number of lines in csv file, including header.
    uint32_t entries = 0;  // Number of entries for the instance.

    int chr;


    // Print header.
    printUntilChar( file, '\n', newFile );
    putc( '\n', newFile );


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
            break;
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
            // Print row and continue.
            fprintf( newFile, "%u,", n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

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
            // Print row and continue.
            fprintf( newFile, "%u,%u,", f, n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

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
            // Print row and continue.
            fprintf( newFile, "%u,%u,%u,", f, t, n );
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );

            continue;
        }


        // This is the row for the instance.

        entries++;

        // If this isn't the first entry, print warning, but proceed.
        if ( entries > 1 )
        {
            fprintf( stderr, "Error: duplicated instance at row %u in csv file. Proceeding anyway.\n\n",
                             rows );
        }

        // Print gods.
        fprintf( newFile, "%u,%u,%u,", f, t, r );

        // Print upper bound.
        printUntilChar( file, ',', newFile );
        putc( ',', newFile );

        // Print status.
        printUntilChar( file, ',', newFile );
        putc( ',', newFile );

        // This is the ind. rep. count; read, increment, print.

        chr = getc(file);

        if ( !isdigit(chr) )
        {
            fprintf( stderr, "Error reading ind. rep. counter on row %u in csv file.\n\n",
                             rows );

            return true;
        }

        getNumber( file, chr, n );
        n++;
        fprintf( newFile, "%u,", n );

        // Print info.
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( s->outFile, "\nindependent replication counter "
                                 "incremented, to: %u\n", n );
        }
        

        // Print notes.
        printUntilChar( file, ',', newFile );
        putc( ',', newFile );


        // Print version and replication command.
        if ( s->noteReplications == 1 )
        {
            // Print version.
            printUntilChar( file, ',', newFile );
            putc( ',', newFile );

            // Print replication command.
            printUntilChar( file, '\n', newFile );
            putc( '\n', newFile );
        }
        else  // Replace old replication command with new.
        {
            fprintf( newFile, "%s,", hardestVersion );
            
            // Here comes the replication command. We'll add seed and -i 0 last,
            // and maybe the bound used.

            for ( int c = 0; c != argc; c++ )
            {
                fprintf( newFile, "%s ", argv[c] );
            }

            fprintf( newFile, "-s %lu -i 0", seed );    

            // Handle bound used.
            if ( s->printBoundUsed )
            {
                if ( boundUsed > 888.8 )
                {
                    fprintf( newFile, " -u 888.8 -M 1" );        
                }
                else
                {
                    fprintf( newFile, " -u %.*g -M 1", DBL_DECIMAL_DIG, boundUsed );        
                }
            }

            // End with CRLF.
            fprintf( newFile, "\r\n" );

            // Skip rest of file line.
            eatLine(file);
        }
    }
    while ( chr != EOF );  // EOF is handled at the start, mostly.


    fclose(file);
    fclose(newFile);

    // Update backup file.

    remove( s->backupBoundsFileName );

    if ( rename( s->boundsFileName, s->backupBoundsFileName ) )
    {
        fprintf( stderr, "error: couldn't rename backup bounds file\n\n" );
    }

    remove( s->boundsFileName );  // In case above failed, and we are on Windows.

    if ( rename( tmpFileName, s->boundsFileName ) )
    {
        fprintf( stderr, "error: couldn't rename temporary bounds file\n\n" );

        return true;
    }


    return false;
}
