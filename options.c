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
"\"The hardest logic puzzle ever\". The algorithm is derived from the paper\n"
"'How to Solve \"The Hardest Logic Puzzle Ever\" and Its Generalization'.\n"
"  Gods are named g0, g1, ...\n"
"  To succinctly reproduce a solution, or see its questions, you can do a long\n"
"search, note the seed, the upper bound, and maybe the best lvl 0 pos estimate,\n"
"for the one best sub-search, and then rerun with that seed (-s), that upper bound (-u),\n"
"and that best lvl 0 pos estimate (-E), and with -v for the questions, and maybe -i 0.\n"
           );

    fprintf( 
stdout, 
"\nExamples:\n" 
"  ./hardest -v -f 0 -t 3 -r 2 -i 0 -b 0 -s 4  reproduces optimal solution, with questions\n"
"  ./hardest -f 1 -t 6 -r 2 -i 5000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13030756753776470731\n"
"    -u 8.299603174603174  reproduces a current upper bound.\n"
"  ./hardest -f 1 -t 5 -r 4 -i 2000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13210365863729163090\n"
"    -u 12.363476562500001 -B 6:1  reproduces an other current upper bound.\n"
"  ./hardest -f 1 -t 3 -r 1 -i 0 -b 0 -v  shows why the solution is optimal,\n"
"    and why all solver solutions to problems with just one random god are optimal.\n"           );

    fprintf(
stdout,
"\nOptions, with defaults in [ ]:\n"
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
"                          will be used. Warning! Set to 0 for harder \n"
"                          problems! Then use -B to allow some more.\n"
"                          Repeats, as implemented, are only meant for the\n"
"                          first few top levels; don't use -b other than -b 0.\n"
"  -B --iterate-sub-search-lvl <unsigned integer>:<unsigned integer>\n"
"                          Like -b but for a level. E.g. -B 1:7 sets lvl 1\n"
"                          to 7. [0:%u, 1:%u, 2:%u, 3:%u, 4:%u, 5:%u, 6:%u, 7:%u, 8:%u, 9:%u, ...]\n"
"  -c --catch-aborts <unsigned integer>\n"
"                          Catch this many aborts in one search. [%lu]\n"
"                          -c 0 will turn off abort catching.\n"
"  -D --mem-inc-factor <float>\n"
"                          Set factor >= 1.1 that memory is to be enlarged\n"
"                          by when it needs to be reallocated. [%f]\n"
"                          Not used; there are no reallocations.\n"
"  -e --abort-leeway-end <float>\n"
"                          abortLeeway becomes abort-leeway-end towards the\n"
"                          end, and never less. [%f]\n"
"                            Warning! Setting this low is risky and might very\n"
"                          well abort tries that would have found an improvement.\n"
"                          You can disable aborts by setting this to say\n"
"                          1.4 or larger.\n"
"  -E --best-lvl-0-pos-est <float>\n"
"                          Set this value. It's used as a threshold for the positive\n"
"                          side, but typically it's only supplied here to reproduce a\n"
"                          search. [%g]\n"
"  -f --false-gods <unsigned integer>\n"
"                          Set number of false gods.\n",
s->abortLeewayStart,
s->dontAbortUntil,
s->lvlReps[0], s->lvlReps[1], s->lvlReps[2], s->lvlReps[3], s->lvlReps[4], 
s->lvlReps[5], s->lvlReps[6], s->lvlReps[7], s->lvlReps[8], s->lvlReps[9],
s->catchAbortsN,
s->memIncFactor,
s->abortLeewayEnd,
s->bestLvl0PosEst
           );

    fprintf(
stdout,
"  -g --good-gods <unsigned integer>\n"
"                          The greater the value, the harder we'll try to\n"
"                          find promising gods to pursue. [%u]\n"
"                          0 means random (by chance) gods.\n"
"                          1 means a linear search.\n"
"                          2 means a quadratic search.\n"
"                          3 means 2 with added virtual perfect swaps,\n"
"                          which could be cubicish.\n"
"                          Only 0 and 1 are implemented so far. >1 means 1.\n"
"  -G --global-bound <unsigne integer>\n"
"                          If 1, we'll use the upper bound globaly. Otherwise\n"
"                          local bounds will be used. [%u]\n"
"  -h --help               Print this message.\n"
"  -H --estimate-heuristic  <unsigned integer>\n"
"                          Set the estimate heuristic used:\n"
"                          0 means a probabilistic update. This might be best?\n"
"                          1 means an average weighing approach. This will be biased low.\n"
"                          2 means counting weights with low probability fully, and\n"
"                            repeatedly. This will be biased high. [%u]\n"
"  -i --iterate <unsigned integer>\n"
"                          The search will be repeated this many times. [%u]\n",
s->findGoodGods,
s->globalBound,
s->estimateHeuristic,
s->iterate
           );

    fprintf(
stdout,
"  -l --resume-aborted-leeway <float>\n"
"                          When the result estimate at a node is <\n"
"                          resume-aborted-leeway * upperBound, aborts\n"
"                          will be caught and search resumed. [%f]\n"
"  -n --gods <unsigned integer>\n"
"                          Set number of total gods.\n"
"                          Only 3 of the 4 god numbers need to be set.\n"
"  -N --odd-bias <unsigned integer>\n"
"                          0 means no bias, random.\n"
"                          1 means positive side will always have one more disjunct\n"
"                            when there is an odd number of disjuncts.\n"
"                          2 means negative side will always have one more disjunct\n"
"                            when there is an odd number of disjuncts. [%u]\n",
//"                          There are indications that positive sides have higher question\n"
//"                          averages, indicating that option 2 might be good, to balance sides. [%u]\n",
s->resumeAbortedLeeway,
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
"  -P --print-info[=no|yes]\n"
"                          Print info if possible. [%s]\n"
"  -F --precision <unsigned integer>\n"
"                          Set precision used when printing floats.\n"
"  --print-options         Print option settings and then quit.\n"
"                          To fine-tune the verbosity, run e.g.\n"
"                          './hardest -v3 --print-options'\n"
"                          and then tune the printed verbosity vector.\n"
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
"  -v --verbose [level]    Set verbosity level (0-9). No arg means 8. [4]\n"
"  --verbosity-vector <unsigned integer>\n"
"                          Set the verbosity vector. [%#llx]\n"
"                          The integer can be bin (0b), hex (0x),\n"
"                          octal (0), or plain decimal.\n"
"  --version               Print the version number.\n",
hi->hard->upperBound,
(unsigned long long int)s->verbosityVector
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



// Prints option settings.
static void printSettings( HardInstance * hi )
{
    Settings * s = hi->settings;

    printf(
"  --print-info=%s",
(s->verbosityVector & HardVerbosity_printInfo) ? "yes" : "no"
          );

    printf(
"  --verbosity-vector=%#llx",
(unsigned long long int) s->verbosityVector
          );

    putc( '\n', stdout );
}



// Parse the command line options.
//   Returns 0 iff everything went fine and you should continue on.
// Returns 1 iff there was an error. Returns > 1 for e.g. --help and you
// should stop.
static int parseCommandLineOptions( HardInstance * hi,
                                    int argC, char * * argV )
{
    #ifndef NoGetopt

    // Tells whether or not an out-file has been specified.
    bool outFileIsSet = false;

    {
        int c;

        int optionIndex;   // Never used.

        static struct option longOptions[] =
        {
            { "dont-abort-until",       required_argument, NULL, 'A' },
            { "iterate-sub-search-lvl", required_argument, NULL, 'B' },
            { "mem-inc-factor",         required_argument, NULL, 'D' },
            { "best-lvl-0-pos-est",     required_argument, NULL, 'E' },
            { "precision",              required_argument, NULL, 'F' },
            { "global-bound",           required_argument, NULL, 'G' },
            { "estimate-heuristic",     required_argument, NULL, 'H' },
            { "odd-bias",               required_argument, NULL, 'N' },
            { "optimize-non-r",         required_argument, NULL, 'O' },
            { "print-info",             optional_argument, NULL, 'P' },
            { "shuffle-conjunctions",   required_argument, NULL, 'R' },
            { "swap",                   required_argument, NULL, 'S' },
            { "indent",                 required_argument, NULL, 'T' },
            { "abort-leeway-start",     required_argument, NULL, 'a' },
            { "iterate-sub-searches",   required_argument, NULL, 'b' },
            { "catch-aborts",           required_argument, NULL, 'c' },
            { "abort-leeway-end",       required_argument, NULL, 'e' },
            { "false-gods",             required_argument, NULL, 'f' },
            { "good-gods",              required_argument, NULL, 'g' },
            { "help",                   no_argument,       NULL, 'h' },
            { "iterate",                required_argument, NULL, 'i' },
            { "resume-aborted-leeway",  required_argument, NULL, 'l' },
            { "gods",                   required_argument, NULL, 'n' },
            { "outfile",                required_argument, NULL, 'o' },
            { "quiet",                  no_argument,       NULL, 'q' },
            { "silent",                 no_argument,       NULL, 'q' },
            { "random-gods",            required_argument, NULL, 'r' },
            { "seed",                   required_argument, NULL, 's' },
            { "true-gods",              required_argument, NULL, 't' },
            { "upper-bound",            required_argument, NULL, 'u' },
            { "verbose",                optional_argument, NULL, 'v' },
            { "version",                no_argument,       NULL, CHAR_MAX+2 },
            { "print-more",             optional_argument, NULL, CHAR_MAX+3 },
            { "print-options",          no_argument,       NULL, CHAR_MAX+4 },
            { "verbosity-vector",       required_argument, NULL, CHAR_MAX+5 },
            { 0, 0, 0, 0 }
        };

        while ( true )
        {
            c = getopt_long( argC, argV,
                             "A:B:D:E:F:G:H:N:O:P::R:S:T:a:b:c:e:f:g:hi:l:n:o:qr:s:t:u:v::",
                             longOptions, &optionIndex );

            if ( c == -1 )
            {
                break;
            }

            switch ( c )
            {
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

                    hi->settings->dontAbortUntil = n;
                }

                break;

            case 'B':
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

                    hi->settings->lvlReps[k] = n;
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
 
                    hi->settings->memIncFactor = max( memIncFactor, 1.1 );
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
 
                    hi->settings->bestLvl0PosEst = x;
                }

                break;

            case 'F':
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

                    hi->settings->precision = n;
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

                    hi->settings->globalBound = n;
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

                    hi->settings->estimateHeuristic = n;
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

                    hi->settings->oddBias = n;
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

                    hi->settings->optimizeNonR = n;
                }

                break;

            case 'P':
                if ( optarg )
                {
                    if ( strcmp( optarg, "no" ) == 0 )
                    {
                        hi->settings->verbosityVector &=
                            ~HardVerbosity_printInfo;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        hi->settings->verbosityVector |=
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
                    hi->settings->verbosityVector |=
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

                    hi->settings->shuffleConjunctions = n;
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

                    hi->settings->doSwaps = n;
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

                    hi->settings->indent = n;
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
 
                    hi->settings->abortLeewayStart = max( r, 0.999 );  // ??
                }

                break;

            case 'b':
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
                        hi->settings->lvlReps[k] = n;
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

                    hi->settings->catchAbortsN = n;
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
 
                    hi->settings->abortLeewayEnd = max( r, 0.998 );  // ??
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

                    hi->settings->findGoodGods = n;
                }

                break;

            case 'h':
                printHelpMessage(hi);

                return 2;

            case 'i':
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

                    hi->settings->iterate = n;
                }

                break;

            case 'l':
                {
                    double r;

                    if ( readReal( optarg, &r ) )
                    {
                        fprintf( stderr,
                                 "\nError: the argument to command line "
                                 "options -l and --resume-aborted-leeway\n"
                                 "must be a real on form '1.2', '3.' or '4'. "
                                 "You supplied %s.\n\n", optarg );

                        return 1;
                    }
 
                    hi->settings->resumeAbortedLeeway = r;
                                                      //min( r, 1.01 );  // ??
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
                outFileIsSet = true;

                if ( strcmp( optarg, "-" ) != 0 )
                {
                    char * name = allocStrCopy(optarg);

                    if ( name == NULL )
                    {
                        fprintf( stderr, "\nError: not enough memory.\n" );

                        return 1;
                    }

                    hi->settings->outFile = fopen( optarg, "w" );
                    hi->settings->outFileName = name;

                    if ( hi->settings->outFile == NULL )
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
                    hi->settings->outFile = stdout;
                    hi->settings->outFileName = NULL;
                }

                break;

            case 'q':
                hi->settings->verbosityVector = 0;

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

                    hi->settings->seed = seed;
                    common_srand(hi->settings->seed);
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
                    //hi->settings->verbosityVector = 0xffffffffffffffff;
                    setVerbosityLevel( hi, 8 );
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
                        hi->settings->verbosityVector &=
                            ~HardVerbosity_printMore;
                    }
                    else if ( strcmp( optarg, "yes" ) == 0 )
                    {
                        hi->settings->verbosityVector |=
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
                    hi->settings->verbosityVector |=
                        HardVerbosity_printMore;
                }

                break;

            case CHAR_MAX+4:
                printSettings( hi );

                return 2;

            case CHAR_MAX+5:
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

                    hi->settings->verbosityVector = vv;
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
        if ( !outFileIsSet )
        {
            if ( strcmp( argV[optind], "-" ) != 0 )
            {
                char * name = allocStrCopy(argV[optind]);

                if ( name == NULL )
                {
                    fprintf( stderr, "\nError: not enough memory.\n" );

                    return 1;
                }

                hi->settings->outFile = fopen( argV[optind], "w" );
                hi->settings->outFileName = name;

                if ( hi->settings->outFile == NULL )
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
                hi->settings->outFile = stdout;
                hi->settings->outFileName = NULL;
            }

            outFileIsSet = true;

            optind++;
        }

        // Any additional argument is an error.
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

            return 1;
        }
    }


    #endif // #ifndef NoGetopt

    return 0;
}



// Returns 0 iff everything went fine and you should continue on.
// Returns 1 iff there was an error. Returns > 1 for e.g. --help and you
// should stop.
int options_parseCommandLineOptions( HardInstance * hi,
                                     int argC, char * * argV )
{
    int result = parseCommandLineOptions( hi, argC, argV );

    #ifndef NoGetopt

    // Reset getopt so that more calls to options_parseCommandLineOptions
    // can be made. An alternative would be to set optind to 1 at the start
    // of parseCommandLineOptions. Best would be though to dump getopt
    // altogether and write something good instead!!
    optind = 1;  // Is this enough???

    #endif // #ifndef NoGetopt

    return result;
}
