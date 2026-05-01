// Module for handling command line options.


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


#include "options.h"
#include "common.h"
#include "hard.h"

// To do: Switch from getopt to Argp?
#ifndef NoGetopt
#include <getopt.h>
#endif

#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <float.h>


#ifndef hardestVersion
#define hardestVersion "<no version number available>"
#endif



// Prints the --help message.
static void printHelpMessage( HardInstance * hi )
{
    Settings * s = hi->settings;
    //Hard     * h = hi->hard;

    fprintf(
stdout,
"\nUsage: hardest [options] [outfile]\n"
"hardest tries to find the fewest questions that solve the generalization of\n"
"The Hardest Logic Puzzle Ever. The algorithm is derived from the paper\n"
"Solving The Hardest Logic Puzzle Ever and its generalizations.\n"
"  Gods are named g0, g1, ...\n"
"  To succinctly reproduce a solution, or see its questions, you can do a long\n"
"search, note the seed, the upper bound, and maybe leaways -a and -e,\n"
"for the one best sub-search, and then rerun with that seed (-s), that upper\n"
"bound (-u), and those leaways, and with -v for the questions, and maybe -i 0.\n"
           );
//"the best lvl 0 pos estimate,\n"
//at best lvl 0 pos estimate (-E), 

    fprintf( 
stdout, 
"\nExamples:\n" 
"  ./hardest -v -f 0 -t 3 -r 2 -i 0 -b 0  reproduces optimal solution, with questions\n"
"  ./hardest -f 2 -t 2 -r 3 -b 3 -B 0:4 -i 999 -a 0.984 -e 0.983 -K 1960 -S 8 -z -99 -U 4 -H 1 "
"-s 12779109849863880712 -i 0 -u 8.9581850000000003 -M 1 -a 0.98399999999999999 -e 0.98299999999999998\n"
"    reproduces a current upper bound.\n"
"  ./hardest -f 1 -t 3 -r 1 -i 0 -b 0 -v  shows why the solution is optimal,\n"
"    and why all solver solutions to problems with just one random god are optimal.\n"
           );
//"  ./hardest -f 0 -t 6 -r 5 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.49 -i 200 -k 11 -b 2 -i 0\n"
//"    -s 12443524816424922443  reproduces a current upper bound.\n"
//"  ./hardest -f 2 -t 3 -r 3 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.0 -i 0\n"
//"    -s 13683506237796025932  reproduces an other current upper bound.\n"

    fprintf(
stdout,
"\nOptions, with defaults in [ ]:\n"
           );

    fprintf(
stdout,
"  -0 --b-floor <unsigned integer>\n"
"                          Like the -b option but the floor, for the milking\n"
"                          mode, -1. The default is -0 0.\n"
"                          -0 b-<c>, e.g. -0 b-1, is acceptable and will set all\n"
"                          floor values to c less than the B values, or to 0\n"
"                          if there is underflow.\n"
"  -1 --milk[=no|yes]      Milk the solution in the bounds file. [%s]\n"
"                          Parameters are based on the solution in the bounds file\n"
"                          but -B combinations upto and including your -B options\n"
"                          are used instead of the -B options in the bounds file,\n"
"                          down to and including level -i.\n"
"                          The -B combinations will not start at 0 but instead use\n"
"                          floors set by the -2 and -0 options.\n"
"                          Any -J option will be ignored.\n"
"                          Options that you supply will be overwritten\n"
"                          by options in the replication command, except these:\n"
"                          -0, -1, -2, -b, -B, -C, -F, -i, -o, -q, -v, -w, and --verbosity-vector.\n"
"  -2 -B-floor <unsigned integer>:<unsigned integer>\n"
"                          Like -0 but for a level. E.g. -2 1:5 sets lvl 1\n"
"                          to 5. [0:%u, 1:%u, 2:%u, 3:%u, 4:%u, 5:%u, 6:%u, 7:%u, 8:%u, 9:%u, ...]\n",
s->milk ? "yes" : "no",
s->lvlRepsFloor[0], s->lvlRepsFloor[1], s->lvlRepsFloor[2], s->lvlRepsFloor[3],
s->lvlRepsFloor[4], s->lvlRepsFloor[5], s->lvlRepsFloor[6], s->lvlRepsFloor[7],
s->lvlRepsFloor[8], s->lvlRepsFloor[9]
           );

    fprintf(
stdout,
"  -a --abort-leeway-start <float>\n"
"                          A search aborts when estimated result >\n"
"                          abortLeeway * upperBound.\n"
"                          abortLeeway = abort-leeway-start at the start. [%f]\n"
"  -A --dont-abort-until <unsigned integer>\n"
"                          Don't abort until at least this many sub-\n"
"                          results are found. [%lu]\n"
"  -b --iterate-sub-search <unsigned integer>\n"
"                          Set number of extra sub-searches; the best result\n"
"                          will be used.\n"
"  -B --iterate-sub-search-lvl <unsigned integer>:<unsigned integer>\n"
"                          Like -b but for a level. E.g. -B 1:7 sets lvl 1\n"
"                          to 7. [0:%u, 1:%u, 2:%u, 3:%u, 4:%u, 5:%u, 6:%u, 7:%u, 8:%u, 9:%u, ...]\n"
"  -c --catch-aborts <unsigned integer>\n"
"                          Catch this many aborts in one search. [%lu]\n"
"                          -c 0 will turn off abort catching.\n"
"  -C --inc-ind-rep-count <unsigned integer>\n"
"                          Set how an independant replication is handled. [%u]\n"
"                          0 means no update.\n"
"                          1 means ind. rep. count will be incremented.\n"
"                          2 means 1 and old replication command will be\n"
"                          replaced with the most recent one.\n"
"  -D --mem-inc-factor <float>\n"
"                          Set factor >= 1.1 that memory is to be enlarged\n"
"                          by when it needs to be reallocated. [%f]\n"
"                          Not used; there are no reallocations.\n"
"  -e --abort-leeway-end <float>\n"
"                          abortLeeway becomes abort-leeway-end towards the\n"
"                          end, and never less. [%f]\n"
"  -E --best-lvl-0-pos-est <float>\n"
"                          Set this value. It's used as a threshold for the positive\n"
"                          side, but typically it's only supplied here to reproduce a\n"
"                          search. Only used if -G 0 is set. [%g]\n"
"  -f --false-gods <unsigned integer>\n"
"                          Set number of false gods.\n"
"  -F --precision <unsigned integer>\n"
"                          Set precision used when printing floats. [%u]\n",
s->abortLeewayStart,
s->dontAbortUntil,
s->lvlReps[0], s->lvlReps[1], s->lvlReps[2], s->lvlReps[3], s->lvlReps[4], 
s->lvlReps[5], s->lvlReps[6], s->lvlReps[7], s->lvlReps[8], s->lvlReps[9],
s->catchAbortsN,
s->noteReplications,
s->memIncFactor,
s->abortLeewayEnd,
s->bestLvl0PosEst,
s->precision
           );

    fprintf(
stdout,
"  -g --good-gods <unsigned integer>\n"
"                          The greater the value, the harder we'll try to\n"
"                          find promising gods to pursue. [%u]\n"
"                          0 means random (by chance) gods.\n"
"                          1 means a linear search.\n"
"                          2 means a search quadratic-cubicish in the -k value.\n"
"                          >2 means 2.\n"
"  -G --global-bound <unsigned integer>\n"
"                          If 1, we'll use the upper bound globally. Otherwise\n"
"                          local bounds will be used. [%u]\n"
"  -h --help               Print this message.\n"
"  -H --estimate-heuristic  <unsigned integer>\n"
"                          Set the estimate heuristic used:\n"
"                          0 means a probabilistic update.\n"
"                          1 means an average weighing approach. This will be biased low.\n"
"                          2 means counting weights with low probability fully, and\n"
"                            repeatedly. This will be biased high. [%u]\n"
"  -i --iterate <unsigned integer>\n"
"                          The search will be repeated this many times. [%u]\n"
"  -I --inc-sample <unsigned integer>\n"
"                          Increase sample size by this each time a new sample\n"
"                          run ends. [%u]\n"
"  -J --note <string without comma and space>\n"
"                          String to be written to the note field in the csv bounds\n"
"                          file if an improvement is found.\n",
s->findGoodGods,
s->globalBound,
s->estimateHeuristic,
s->iterate,
s->minSampleInc
           );

    fprintf(
stdout,
"  -k --good-gods-candidates <unsigned integer>\n"
"                          Number of gods that will be (extra) considered in the search\n"
"                          for good gods to ask. [%u]\n"
"  -K --abort-promille-goal <unsigned integer>\n"
"                          We'll aim to get this many promille aborts. [%u]\n"
"                          Anything above 1000 will turn this off.\n"
"  -l --resume-aborted-leeway <float>\n"
"                          When the result estimate at a node is <\n"
"                          resume-aborted-leeway * upperBound, aborts\n"
"                          will be caught and search resumed. [%g]\n"
"                          -1 means that the -a value is used. -2 means the -e value.\n"
"  -L --top-local-reset-level <unsigned integer>\n"
"                          The top level where we'll switch from a global hash\n"
"                          reset to a local reset. [%u]\n"
"  -m --print-used-bound[=no|yes]\n"
"                          Add the bound used to the replication command. [%s]\n"
"  -M --use-file-bound <unsigned integer>\n"
"                          Set how the upper bound from the bounds file is used. [%u]\n"
"                          0 means do nothing.\n"
"                          1 means use bound from file if upper bound is undefined.\n"
"                          2 means use bound from file if it's smaller than current upper bound.\n"
"  -n --gods <unsigned integer>\n"
"                          Set number of total gods.\n"
"                          Only 3 of the 4 god numbers need to be set;\n"
"                          if you read options from the bounds file,\n"
"                          then you need to set -f, -t, and -r, first.\n"
"  -N --odd-bias <unsigned integer>\n"
"                          0 means no bias, random.\n"
"                          1 means positive side will always have one more disjunct\n"
"                            when there is an odd number of disjuncts.\n"
"                          2 means negative side will always have one more disjunct\n"
"                            when there is an odd number of disjuncts. [%u]\n",
s->goodGodsCandN,
s->abortPromilleGoal,
s->resumeAbortedLeeway,
s->topLocalResetLevel,
s->printBoundUsed ? "yes" : "no",
s->useBoundFromFile,
s->oddBias
           );

    fprintf(
stdout,
"  -o --outfile <file>     Specify an output file. - means stdout. [stdout]\n"
           );

    fprintf(
stdout,
"  -O --optimize-non-r <unsigned integer>\n"
"                          The greater the value, the harder we'll try to\n"
"                          find promising disjuncts to assign shorter paths\n"
"                          when a non-random god has been found.[%u]\n"
"                          0 means assignment of floor logs in order, with no extra search.\n"
"                          1 means that some promising disjuncts get floor logs before rest.\n"
"                          n means doing n optimization iterations.\n"
"                          Should not be very low.\n",
s->optimizeNonR
           );

    fprintf(
stdout,
"  -p --print-placeholders <unsigned integer>-<unsigned integer>\n"
"                          Generate placeholders for the upper bounds file,\n"
"                          from where number of gods sums up to first value,\n"
"                          to, and including, where gods sum up to second value.\n"
"                          Prints to stdout since it could be bad to append the bounds file:\n"
"                          Try to ensure that a placeholder don't come after a real\n"
"                          upper bound of the same problem instance in the bounds file,\n"
"                          even though that shouldn't matter at the moment; the lowest\n"
"                          bound will be retained regardless.\n"
"  -P --print-info[=no|yes]\n"
"                          Print info if possible. [%s]\n"
"  --print-options         Print option settings and then quit.\n"
"  --print-more[=no|yes]   Print more info. [%s]\n"
"  -q --quiet --silent     Run silently; only print error messages.\n",
(s->verbosityVector & HardVerbosity_printInfo) ? "yes" : "no",
(s->verbosityVector & HardVerbosity_printMore) ? "yes" : "no"
           );

    fprintf(
stdout,
"  -r --random-gods <unsigned integer>\n"
"                          Set number of random gods.\n"
"  -R --shuffle-conjunctions <unsigned integer>\n"
"                          Set amount of shuffling. [%u]\n"
"                          0 means no shuffling.\n"
"                          1 means only non-g0=R conjunctions are shuffled.\n"
"                          2 means only g0=R conjunctions are shuffled.\n"
"                          3 means all conjunctions are shuffled (within their partitions).\n"
"                          4 means also non-gq=R conjunctions are shuffled at every recursive call.\n"
"                          5 means 3 and that gq=R conjunctions are shuffled at every recursive call.\n"
"                          6 means all of the above.\n"
"  -s --seed <seed>        Set the seed. [current time]\n"
"  -S --swap <unsigned integer>\n"
"                          Set amount of swapping to be attempted to try\n"
"                          to get to-be-asked gods gi and gj non-random. [%u]\n"
"                          0 means no conjunction swaps.\n"
"                          1 means only perfect swaps.\n" 
"                          2 means also upper non-perfect swaps.\n"
"                          3 means also lower non-perfect swaps.\n"
"                          4 means also 0111 non-perfect swaps.\n"
"                          5 means also 1101 non-perfect swaps.\n"
"                          6 means also \"non-improving\" swaps, to balance, \n"
"                          but not for sides with no randoms.\n"
"                          7 means also balancing always.\n"
"                          8 means 6 but also active \"neutral\" or \"non-improving\"\n"
"                          swaps that unbalance sides if they get a side to 0 randoms.\n"
"  -t --true-gods <unsigned integer>\n"
"                          Set number of true gods.\n"
"  -T --indent <unsigned integer>\n"
"                          Indent this much times the question depth when printing. [%u]\n",
s->shuffleConjunctions,
s->doSwaps,
s->indent
           );



    fprintf(
stdout,
"  -u --upper-bound <float>\n"
"                          A search aborts when estimated result >\n"
"                          abort-leeway times this -u value. But this -u\n"
"                          value will be updated if a better result is\n"
"                          found. [%g]\n"
"  -U --max-unbal <unsigned integer>\n"
"                          The sum of randoms can be at most this for\n"
"                          unbalancing strategies to be deployed. [%lu]\n"
"  -v --verbose [level]    Set verbosity level (0-9). No arg means 8. [6]\n"
"  --verbosity-vector <unsigned integer>\n"
"                          Set the verbosity vector. [%#llx]\n"
"                          The integer can be bin (0b), hex (0x),\n"
"                          octal (0), or plain decimal.\n"
"  --version               Print the version number, %s.\n"
"  -V --rebalance[=no|yes]\n"
"                          If one side both has more disjuncts and randoms, then\n"
"                          we'll rebalance, if this value is true. [%s]\n"
"  -w --update-bounds[=no|yes]\n"
"                          If true, then improved upper bounds are written to\n"
"                          the csv bounds file. [%s]\n"
"  -W --estimate-weight <float>\n"
"                          In estimate heuristic 1, factor for new results will\n"
"                          get upped by this, and factor for old average will be\n"
"                          lowered by this, when taking their average, when\n"
"                          calculating a new estimate. [%g]\n"
"  -X --change-factor <float>\n"
"                          Determines how much abort values will change. [%g]\n"
"  -Y --abort-leeway-change <float>\n"
"                          We'll add or subtract this from abort-leeway-start and -end,\n"
"                          adjusted, to achive abort-promille-goal. [%g]\n"
"  -z --CIz <float>        Set z-value for CIs. Default is 95%% CIs. [%f]\n"
"                          -99 is interpreted as the z-value for 99%% CIs. -80, -90,\n"
"                          -95, -99, -999 are also supported.\n"
"  -Z --min-sample-size <unsigned integer>\n"
"                          The minimal sample size in order to update abort heuristics.\n"
"                          If 0, then we'll try to set it to something reasonable,\n"
"                          conservatively. [%u]\n",
hi->hard->upperBound,
s->maxUnbal,
(unsigned long long int)s->verbosityVector,
hardestVersion,
s->rebalance ? "yes" : "no",
s->updateBoundsFile ? "yes" : "no",
s->estWeight,
s->changeFactor,
s->abortLeewayChange,
s->ciz,
s->minSampleSize
           );

    fprintf(
stdout,
"\nFor more on what the options mean, see hard.h, and the paper above.\n"
           );

    fprintf(
stdout,
"\nhardest is open source licensed under the Reciprocal Public\n"
"License, version 1.1; Copyright (C) 2026 Daniel Vallstrom.\n"
           );

    fprintf(
stdout,
"\nSend bug reports, feedback, etc. to daniel.vallstrom@gmail.com.\n\n"
           );
    fprintf(
stdout,
"If you find an improvement on an upper bound, please tell! E.g. at:\n"
"https://github.com/DanielVallstrom/hardest\n\n"
           );
}



// Sets the verbosity level.
static void setVerbosityLevel( HardInstance * hi, unsigned int vl )
{
    HardVerbosityVector vv = 0;

    switch ( vl )
    {

    // All cases are fall-throughs:

    default:

    case 9: 
    case 8:
        vv |= HardVerbosity_printAll;
    case 7:
        vv |= HardVerbosity_printMore;
    case 6:
        vv |= HardVerbosity_printTime;
    case 5:
        vv |= HardVerbosity_printInfo;
    case 4:
        vv |= HardVerbosity_printSeed;
    case 3:
    case 2:
        vv |= HardVerbosity_printResult;
    case 1:
        vv |= HardVerbosity_printErrors;
    case 0:

        break;
    }

    hi->settings->verbosityVector = vv;
}



// Prints settings.
void options_printSettings( HardInstance * hi )
{
    Settings * s = hi->settings;
    FILE * f = s->outFile;

    fprintf( f, "settings:\n" );
    
    fprintf( f, "outfile: %s\n", f == stdout ? "stdout" : 
             ( f == stderr ? "stderr" : s->outFileName ) );
    fprintf( f, "precision: %u\n", s->precision );
    fprintf( f, "seed (-s): %lu\n", s->seed );
    fprintf( f, "swapping (-S): %u\n", s->doSwaps );
    fprintf( f, "good-gods (-g): %u\n", s->findGoodGods );
    fprintf( f, "optimize-non-r (-O): %u\n", s->optimizeNonR );
    fprintf( f, "shuffle-conjunctions (-R): %u\n", s->shuffleConjunctions );
    fprintf( f, "odd-bias (-N): %u\n", s->oddBias );
    fprintf( f, "iterate (-i): %u\n", s->iterate );
    fprintf( f, "abort-leeway-start (-a): %g\n", s->abortLeewayStart );
    fprintf( f, "abort-leeway-end   (-e): %g\n", s->abortLeewayEnd );
    fprintf( f, "dont-abort-until (-A): %lu\n", s->dontAbortUntil );
    fprintf( f, "resume-aborted-leeway (-l): %g\n", s->resumeAbortedLeeway );
    fprintf( f, "catch-aborts (-c): %lu\n", s->catchAbortsN );
    fprintf( f, "number of levels that are caught: %u\n", s->maxCatchDepth );

    // Print -B values. 
    for ( uint8_t k = 0; k != 20; k++ )
    {
        fprintf( f, " -B %u:%u", k, s->lvlReps[k] );
    }
    fprintf( f, " ...\n" );

    fprintf( f, "indent (-T): %u \n", s->indent );
    fprintf( f, "estimate-heuristic (-H): %u\n", s->estimateHeuristic );
    fprintf( f, "best-lvl-0-pos-est (-E): %g\n", s->bestLvl0PosEst );
    fprintf( f, "topLocalResetLevel (-L): %u\n", s->topLocalResetLevel );
    fprintf( f, "globalBound (-G): %u\n", s->globalBound );
    fprintf( f, "goodGodsCandN (-k): %u\n", s->goodGodsCandN );
    fprintf( f, "max-unbal (-U): %lu\n", s->maxUnbal );
    fprintf( f, "estWeight (-W): %g\n", s->estWeight );

    fprintf( f, "bounds file: %s\n", s->boundsFileName );
    fprintf( f, "updateBoundsFile (-w): %u\n", s->updateBoundsFile );
    fprintf( f, "upper bound in csv file: %g\n", s->upperBoundInFile );
    fprintf( f, "bound status in csv file: %u\n", s->boundStatus );
    fprintf( f, "backup bounds file: %s\n", s->backupBoundsFileName );

    // Print command line.
    fprintf( f, "command line:\n" );
    for ( int c = 0; c != s->argC; c++ )
    {
        fprintf( f, "%s ", s->argV[c] );
    }
    fputc( '\n', f );

    fprintf( f, "useBoundFromFile (-M): %u\n", s->useBoundFromFile );
    fprintf( f, "printBoundUsed (-m): %u\n", s->printBoundUsed );
    fprintf( f, "noteReplications (-C): %u\n", s->noteReplications );
    fprintf( f, "boundsFileSeed: %lu\n", s->boundsFileSeed );
    fprintf( f, "abortPromilleGoal (-K): %u\n", s->abortPromilleGoal );
    fprintf( f, "abortLeewayChange (-Y): %g\n", s->abortLeewayChange );
    fprintf( f, "changeFactor (-X): %g\n", s->changeFactor );
    fprintf( f, "minSampleSize (-Z): %u\n", s->minSampleSize );
    fprintf( f, "minSampleInc (-I): %u\n", s->minSampleInc );
    fprintf( f, "ciz (-z): %g\n", s->ciz );
    fprintf( f, "note (-J): %s\n", s->note != NULL ? s->note : "" );
    fprintf( f, "rebalance (-V): %u\n", s->rebalance );
    fprintf( f, "milk (-1): %u\n", s->milk );

    // Print reproduction command.
    fprintf( f, "reproduction command:\n" );
    for ( int c = 0; c != s->argCRep; c++ )
    {
        fprintf( f, "%s ", s->argVRep[c] );
    }
    fputc( '\n', f );

    if ( s->lvlRepsOrig != NULL )
    {
        fprintf( f, "original B values:\n" );
        for ( uint8_t k = 0; k != 20; k++ )
        {
            fprintf( f, " -B %u:%u", k, s->lvlRepsOrig[k] );
        }
        fprintf( f, " ...\n" );
    }

    if ( s->lvlRepsFloor != NULL )
    {
        fprintf( f, "B floor values (-0 or -2):\n" );
        for ( uint8_t k = 0; k != 20; k++ )
        {
            fprintf( f, " -2 %u:%u", k, s->lvlRepsFloor[k] );
        }
        fprintf( f, " ...\n" );
    } 

    fprintf( f, "upper bound (state ('h')) (-u): %g\n\n", 
             hi->hard->upperBound );


    /*
    printf(
"  --print-info=%s",
(s->verbosityVector & HardVerbosity_printInfo) ? "yes" : "no"
          );

    printf(
"  --verbosity-vector=%#llx",
(unsigned long long int) s->verbosityVector
          );

    putc( '\n', stdout );
    */
}



// Generates placeholders for upper bounds file, from where number of
// gods sums up to lowSum, to, and including, where gods sum up to highSum.
//   Appends the bounds file. Not, since it's dangerous. Goes to stdout instead.
static bool printPlaceholders( /*HardInstance * hi,*/ GodsN lowSum, GodsN highSum )
{
    if ( lowSum > highSum  ||  highSum < 3 )
    {
        return false;
    }

    lowSum = max( lowSum, 3 );

    // Open the bounds file.
    //FILE * file = fopen( hi->settings->boundsFileName, "a" );

    /*
    if ( file == NULL )
    {
        fprintf( stderr, "\nCould not open best_known_bounds.csv\n\n" );

        return true;
    }
    */

    for ( GodsN n = lowSum; n != highSum + 1; n++ )
    {
        for ( GodsN r = 1; r != (n-1) / 2 + 1; r++ )
        {
            for ( GodsN t = n-r, f = 0; f <= t; t--, f++ )
            {
                fprintf( stdout /*file*/, "%u,%u,%u,999.9,upper_bound,placeholder,,\r\n",
                                f, t, r );
            }
        }
    }

    //fclose(file);

    return false;
}



// Parse the command line options.
//   Returns 0 iff everything went fine and you should continue on.
// Returns 1 iff there was an error. Returns > 1 for e.g. --help and you
// should stop.
//   mode should be 0 in first, normal, call. In further calls, set mode
// to > 0.
static int parseCommandLineOptions( HardInstance * hi,
                                    int argC, char * * argV, uint8_t mode )
{
    #ifndef NoGetopt

    Settings * s = hi->settings;

    // Tells whether or not an out-file has been specified.
    bool outFileIsSet = false;

    {
        int c;

        int optionIndex;   // Never used.

        static struct option longOptions[] =
        {
            { "b-floor",                required_argument, NULL, '0' },
            { "milk",                   optional_argument, NULL, '1' },
            { "B-floor",                required_argument, NULL, '2' },
            { "dont-abort-until",       required_argument, NULL, 'A' },
            { "iterate-sub-search-lvl", required_argument, NULL, 'B' },
            { "inc-ind-rep-count",      required_argument, NULL, 'C' },
            { "mem-inc-factor",         required_argument, NULL, 'D' },
            { "best-lvl-0-pos-est",     required_argument, NULL, 'E' },
            { "precision",              required_argument, NULL, 'F' },
            { "global-bound",           required_argument, NULL, 'G' },
            { "estimate-heuristic",     required_argument, NULL, 'H' },
            { "inc-sample",             required_argument, NULL, 'I' },
            { "note",                   required_argument, NULL, 'J' },
            { "abort-promille-goal",    required_argument, NULL, 'K' },
            { "top-local-reset-level",  required_argument, NULL, 'L' },
            { "use-file-bound",         required_argument, NULL, 'M' },
            { "odd-bias",               required_argument, NULL, 'N' },
            { "optimize-non-r",         required_argument, NULL, 'O' },
            { "print-info",             optional_argument, NULL, 'P' },
            { "shuffle-conjunctions",   required_argument, NULL, 'R' },
            { "swap",                   required_argument, NULL, 'S' },
            { "indent",                 required_argument, NULL, 'T' },
            { "max-unbal",              required_argument, NULL, 'U' },
            { "rebalance",              optional_argument, NULL, 'V' },
            { "estimate-weight",        required_argument, NULL, 'W' },
            { "change-factor",          required_argument, NULL, 'X' },
            { "abort-leeway-change",    required_argument, NULL, 'Y' },
            { "min-sample-size",        required_argument, NULL, 'Z' },
            { "abort-leeway-start",     required_argument, NULL, 'a' },
            { "iterate-sub-searches",   required_argument, NULL, 'b' },
            { "catch-aborts",           required_argument, NULL, 'c' },
            { "abort-leeway-end",       required_argument, NULL, 'e' },
            { "false-gods",             required_argument, NULL, 'f' },
            { "good-gods",              required_argument, NULL, 'g' },
            { "help",                   no_argument,       NULL, 'h' },
            { "iterate",                required_argument, NULL, 'i' },
            { "good-gods-candidates",   required_argument, NULL, 'k' },
            { "resume-aborted-leeway",  required_argument, NULL, 'l' },
            { "print-used-bound",       optional_argument, NULL, 'm' },
            { "gods",                   required_argument, NULL, 'n' },
            { "outfile",                required_argument, NULL, 'o' },
            { "print-placeholders",     required_argument, NULL, 'p' },
            { "quiet",                  no_argument,       NULL, 'q' },
            { "silent",                 no_argument,       NULL, 'q' },
            { "random-gods",            required_argument, NULL, 'r' },
            { "seed",                   required_argument, NULL, 's' },
            { "true-gods",              required_argument, NULL, 't' },
            { "upper-bound",            required_argument, NULL, 'u' },
            { "verbose",                optional_argument, NULL, 'v' },
            { "update-bounds",          optional_argument, NULL, 'w' },
            { "CIz",                    required_argument, NULL, 'z' },
            { "version",                no_argument,       NULL, CHAR_MAX+2 },
            { "print-more",             optional_argument, NULL, CHAR_MAX+3 },
            { "print-options",          no_argument,       NULL, CHAR_MAX+4 },
            { "verbosity-vector",       required_argument, NULL, CHAR_MAX+5 },
            { 0, 0, 0, 0 }
        };

        while ( true )
        {
            c = getopt_long( argC, argV,
                             "0:1::2:"
                             "A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P::R:S:T:U:V::W:X:Y:Z:"
                             "a:b:c:e:f:g:hi:k:l:m::n:o:p:qr:s:t:u:v::w::z:",
                             longOptions, &optionIndex );

            if ( c == -1 )
            {
                break;
            }

            switch ( c )
            {
            case '0':
                if ( mode == 0 )
                {
                    unsigned int n;
                    bool offsetMode = *optarg == 'b';

                    if ( *optarg == 'b' )
                    {
                        if ( optarg[1] == '-' )
                        {
                            optarg += 2;
                        }
                        else
                        {
                            fprintf( stderr,
                                     "\nError: the argument to command line "
                                     "options -0 and --b-floor must be\n"
                                     "an unsigned integer, or b-<c>. "
                                     "You supplied %s.\n\n", optarg );
                        
                            return 1;
                        }
                    }

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -0 and --b-floor must be\n"
                                 "an unsigned integer, or b-<c>. "
                                 "You supplied %s%s.\n\n", 
                                 offsetMode ? "b-" : "", optarg );

                        return 1;
                    }

                    if ( offsetMode )
                    {
                        for ( uint16_t k = 0; k != MaxDepth; k++ )
                        {
                            if ( s->lvlReps[k] >= n )
                            {
                                s->lvlRepsFloor[k] = s->lvlReps[k] - n;
                            }
                            else
                            {
                                s->lvlRepsFloor[k] = 0;
                            }
                        }
                    }
                    else
                    {
                        for ( uint16_t k = 0; k != MaxDepth; k++ )
                        {
                            s->lvlRepsFloor[k] = n;
                        }
                    }
                }
            
                break;

            case '1':
                if ( mode > 0 )
                {
                    break;
                }

                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->milk = false;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->milk = true;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -1 and --milk must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->milk = true;
                }

                break;

            case '2':
                if ( mode == 0 )
                {
                    unsigned int n;
                    unsigned int k;

                    if ( readUintCharUint( optarg, &k, ':', &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -2 and --B-floor must be\n"
                                 "<unsigned integer>:<unsigned integer>. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->lvlRepsFloor[k] = n;
                }

                break;

            case 'A':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -A and --dont-abort-until must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->dontAbortUntil = n;
                }

                break;

            case 'B':
                if ( mode == 0 )
                {
                    unsigned int n;
                    unsigned int k;

                    if ( readUintCharUint( optarg, &k, ':', &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -B and --iterate-sub-search-lvl must be\n"
                                 "<unsigned integer>:<unsigned integer>. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->lvlReps[k] = n;
                }

                break;

            case 'C':
                if ( mode == 0 )
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -C and --inc-ind-rep-count must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->noteReplications = n;
                }

                break;

            case 'D':
                {
                    double memIncFactor;

                    if ( readReal( optarg, &memIncFactor ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -D and --mem-inc-factor\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->memIncFactor = max( memIncFactor, 1.1 );
                }

                break;

            case 'E':
                {
                    double x;

                    if ( readReal( optarg, &x ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -E and --best-lvl-0-pos-est\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->bestLvl0PosEst = x;
                }

                break;

            case 'F':
                if ( mode == 0 )
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -F and --precision must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->precision = n;
                }

                break;

            case 'G':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -G and --global-bound must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->globalBound = n;
                }

                break;

            case 'H':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -H and --estimate-heuristic must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->estimateHeuristic = n;
                }

                break;

            case 'I':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -I and --inc-sample must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->minSampleInc = n;
                }

                break;

            case 'J':
                if ( mode == 0 )
                {
                    if ( strchr( optarg, ',' )  != NULL  ||
                         strchr( optarg, ' ' )  != NULL  ||
                         //strchr( optarg, '-' )  != NULL  ||
                         strchr( optarg, '\t' ) != NULL )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -J and --note must be\n"
                                 "a string without commas and spaces. "
                                 "You supplied: %s.\n\n", optarg );

                        return 1;
                    }

                    s->note = optarg;
                }

                break;

            case 'K':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -K and --abort-promille-goal must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->abortPromilleGoal = n;
                }

                break;

            case 'L':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -L and --top-local-reset-level must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->topLocalResetLevel = n;
                }

                break;

            case 'M':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -M and --use-file-bound must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->useBoundFromFile = n;
                }

                break;

            case 'N':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -N and --odd-bias must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->oddBias = n;
                }

                break;

            case 'O':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -O and --optimize-non-r must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->optimizeNonR = n;
                }

                break;

            case 'P':
                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->verbosityVector &=
                            ~HardVerbosity_printInfo;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->verbosityVector |=
                            HardVerbosity_printInfo;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -P and --print-info must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->verbosityVector |=
                        HardVerbosity_printInfo;
                }

                break;

            case 'R':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -R and --shuffle-conjunctions must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->shuffleConjunctions = n;
                }

                break;

            case 'S':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -S and --swap must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->doSwaps = n;
                }

                break;

            case 'T':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -T and --indent must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->indent = n;
                }

                break;

            case 'U':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -U and --max-unbal must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->maxUnbal = n;
                }

                break;

            case 'V':
                if ( mode > 0 )  // This could be needed for replication.
                {
                    //break;
                }
                
                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->rebalance = false;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->rebalance = true;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -V and --rebalance must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->rebalance = true;
                }

                break;

            case 'W':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -W and --estimate-weight\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->estWeight = r;
                }

                break;

            case 'X':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -X and --change-factor\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->changeFactor = r;
                }

                break;

            case 'Y':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -Y and --abort-leeway-change\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->abortLeewayChange = r;
                }

                break;

            case 'Z':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -Z and --min-sample-size must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->minSampleSize = n;
                }

                break;

            case 'a':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -a and --abort-leeway-start\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->abortLeewayStart = max( r, 0.8 );  // ??
                }

                break;

            case 'b':
                if ( mode == 0 )
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -b and --iterate-sub-searches must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    for ( uint16_t k = 0; k != MaxDepth; k++ )
                    {
                        s->lvlReps[k] = n;
                    }
                }

                break;

            case 'c':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -c and --catch-aborts must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->catchAbortsN = n;
                }

                break;

            case 'e':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -e and --abort-leeway-end\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->abortLeewayEnd = max( r, 0.7 );  // ??
                }

                break;

            case 'f':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -f and --false-gods must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    hi->hard->fGodsN = n;
                }

                break;

            case 'g':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -g and --good-gods must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->findGoodGods = n;
                }

                break;

            case 'h':
                printHelpMessage(hi);

                return 2;

            case 'i':
                if ( mode == 0 )
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -i and --iterate must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->iterate = n;
                }

                break;

            case 'k':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -k and --good-gods-candidates must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->goodGodsCandN = n;
                }

                break;

            case 'l':
                {
                    double r;

                    if ( readSignedReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -l and --resume-aborted-leeway\n"
                                 "must be a real on form '1.2', '3.' or '4', "
                                 "or -1 or -2. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    s->resumeAbortedLeeway = r;     //min( r, 1.01 );  // ??
                }

                if ( s->resumeAbortedLeeway <= 0   &&
                     s->resumeAbortedLeeway != -1  &&
                     s->resumeAbortedLeeway != -2 )
                {
                    fprintf( stderr,
                             "\nError: the argument to command line "
                              "options -l and --resume-aborted-leeway\n"
                              "must be > 0, or e.g. -1. You supplied %g.\n\n", 
                              s->resumeAbortedLeeway );

                    return 1;
                }

                break;

            case 'm':
                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->printBoundUsed = false;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->printBoundUsed = true;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -m and --print-used-bound must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->printBoundUsed = true;
                }

                break;

            case 'n':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -n and --gods must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    hi->hard->godsN = n;
                }

                break;

            case 'o':
                if ( mode > 0 )
                {
                    break;
                }

                outFileIsSet = true;

                if ( strcmp( optarg, "-" ) != 0 )
                {
                    char * name = allocStrCopy(optarg);

                    if ( name == NULL )
                    {
                        fprintf( stderr, "\nError: not enough memory.\n" );

                        return 1;
                    }

                    s->outFile = fopen( optarg, "w" );
                    s->outFileName = name;

                    if ( s->outFile == NULL )
                    {
                        fprintf( stderr, "\nError: output file %s couldn't "
                                         "be opened.\n", optarg );
                        perror( NULL );
                        putc( '\n', stderr );

                        return 1;
                    }
                }
                else
                {
                    s->outFile = stdout;
                    s->outFileName = NULL;
                }

                break;

            case 'p':
                {
                    unsigned int n;
                    unsigned int k;

                    if ( readUintCharUint( optarg, &k, '-', &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -p and --print-placeholders must be\n"
                                 "<unsigned integer>-<unsigned integer>. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    printPlaceholders( /*hi,*/ k, n );
                }

                return 2;

            case 'q':
                if ( mode == 0 )
                {
                    s->verbosityVector = 0;
                }

                break;

            case 'r':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -r and --random-gods must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    hi->hard->rGodsN = n;
                }

                break;

            case 's':
                {
                    unsigned long long int seed;

                    if ( readULLInt( optarg, &seed ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -s and --seed must be an\n"
                                 "unsigned long integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->seed = seed;
                    common_srand(s->seed);
                }

                break;

            case 't':
                {
                    unsigned int n;

                    if ( readUInt( optarg, &n ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -t and --true-gods must be\n"
                                 "an unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    hi->hard->tGodsN = n;
                }

                break;

            case 'u':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -u and --upper-bound\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    hi->hard->upperBound = r;
                }

                break;

            case 'v':
                if ( mode > 0 )
                {
                    break;
                }

                if ( optarg )
                {
                    unsigned int vl;

                    if ( readUInt( optarg, &vl ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -v and --verbose must be an\n"
                                 "unsigned integer. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    setVerbosityLevel( hi, vl );
                }
                else
                {
                    //s->verbosityVector = 0xffffffffffffffff;
                    setVerbosityLevel( hi, 8 );
                }

                break;

            case 'w':
                if ( mode > 0 )
                {
                    break;
                }

                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->updateBoundsFile = false;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->updateBoundsFile = true;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -w and --update-bounds must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->updateBoundsFile = true;
                }

                break;

            case 'z':
                {
                    double ciz;

                    if ( readSignedReal( optarg, &ciz ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -z and --CIz\n"
                                 "must be a real on form '1.2', '3.' or '4', "
                                 "or '-99'. You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    if ( ciz == -80 )
                    {
                        s->ciz = 1.282;
                    } 
                    else if ( ciz == -85 )
                    {   
                        s->ciz = 1.440;
                    }
                    else if ( ciz == -90 )
                    {   
                        s->ciz = 1.645;
                    } 
                    else if ( ciz == -95 )
                    {
                        s->ciz = 1.96;
                    }
                    else if ( ciz == -99 )
                    {
                        s->ciz = 2.576;
                    }
                    else if ( ciz == -999 )
                    {
                        s->ciz = 3.291;
                    }
                    else
                    {
                        s->ciz = ciz;
                    }
                }

                if ( s->ciz <= 0 )
                {
                    fprintf( stderr,
                             "\nError: the argument to command line "
                              "options -z and --CIz\n"
                              "must be > 0, or e.g. -99. You supplied %g.\n\n", 
                              s->ciz );

                    return 1;
                }

                break;


            case CHAR_MAX+2:
                printf( "hardest %s\n", hardestVersion );

                return 2;

            case CHAR_MAX+3:
                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        s->verbosityVector &=
                            ~HardVerbosity_printMore;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        s->verbosityVector |=
                            HardVerbosity_printMore;
                    }
                    else
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "option --print-more must be\n"
                                 "no or yes. You supplied %s.\n\n", optarg );

                        return 1;
                    }
                }
                else
                {
                    s->verbosityVector |=
                        HardVerbosity_printMore;
                }

                break;

            case CHAR_MAX+4:
                options_printSettings( hi );

                return 2;

            case CHAR_MAX+5:
                if ( mode == 0 )
                {
                    unsigned long long int vv;

                    if ( readULLIntBase( optarg, &vv ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "option --verbosity-vector must be\n"
                                 "an integer on form '0b1101', '0xd' "
                                 "'015' or '13'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }

                    s->verbosityVector = vv;
                }

                break;

            default:
                fprintf( stderr, "\nError: getopt returned %d.\n", c );

                return 1;
            }
        }
    }


    // Handle leftover arguments. If no out file has been specified, the
    // first leftover argument is assumed to be that file.
    if ( optind < argC )
    {
        if ( !outFileIsSet  &&  mode == 0 )
        {
            if ( strcmp( argV[optind], "-" ) != 0 )
            {
                char * name = allocStrCopy(argV[optind]);

                if ( name == NULL )
                {
                    fprintf( stderr, "\nError: not enough memory.\n" );

                    return 1;
                }

                s->outFile = fopen( argV[optind], "w" );
                s->outFileName = name;

                if ( s->outFile == NULL )
                {
                    fprintf( stderr, "\nError: file %s couldn't "
                                     "be opened.\n", argV[optind] );
                    perror( NULL );
                    putc( '\n', stderr );

                    return 1;
                }
            }
            else
            {
                s->outFile = stdout;
                s->outFileName = NULL;
            }

            outFileIsSet = true;

            optind++;
        }

        // Any additional argument is an error.
        //   However, we'll not return error if mode > 0; there an unflagged
        // outfile could be here, maybe(?), and not be an error.
        if ( optind != argC )
        {
            fprintf( stderr, "\nError: non-option argv elements "
                             "(remember no space between flags, e.g. 'v', "
                             "and optional arguments):\n" );

            for ( ; optind != argC; optind++ )
            {
                fprintf( stderr, "  %s\n", argV[optind]);
            }

            fprintf( stderr, "See hardest --help.\n\n" );

            if ( mode == 0 )
            {
                return 1;
            }
        }
    }


    #endif // #ifndef NoGetopt
    

    return 0;
}



// Returns 0 iff everything went fine and you should continue on.
// Returns 1 iff there was an error. Returns > 1 for e.g. --help and you
// should stop.
//   Also saves the command line in case we want to write it to the 
// bounds file.
//   mode should be 0 in first, normal, call. In further calls, set mode
// to > 0.
int options_parseCommandLineOptions( HardInstance * hi,
                                     int argC, char * * argV, uint8_t mode )
{
    int result = parseCommandLineOptions( hi, argC, argV, mode );

    if ( mode == 0 )
    {
        hi->settings->argC = argC;
        hi->settings->argV = argV;
    }

    #ifndef NoGetopt

    // Reset getopt so that more calls to options_parseCommandLineOptions
    // can be made. An alternative would be to set optind to 1 at the start
    // of parseCommandLineOptions. Best would be though to dump getopt
    // altogether and write something else instead.

    #ifdef __GNU_LIBRARY__
    optind = 0;  // Use 0 for GNU systems to re-trigger initialization.
    #else
    optind = 1; // POSIX standard.
    #endif

    #if defined(__FreeBSD__) || defined(__APPLE__)
    optreset = 1;  // Required for BSD and macOS.
    #endif

    #endif  // #ifndef NoGetopt

    return result;
}
