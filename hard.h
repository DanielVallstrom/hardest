// Module for finding the fewest questions to solve the generalized
// "hardest logic puzzle ever".


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


#ifndef hard_H
#define hard_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Forward declarations:
typedef struct Hard_ Hard;
typedef uint8_t God;
typedef struct HardInstance_ HardInstance;

#include "conjHash.h"



// Precision used when printing floats.
#define DefaultPrecision (6)

// The default size of qPath. See below.
#define DefaultqPathSize (300)


// Types. -------------------------------------------------------------

// God types will be coded using 4 bits. You could use some ternary coding,
// ... g_n*3^n + (g_n-1)*3^n-1 + ... + g_1*3^1 + g_0*3^0,
// where g_i is, e.g., 0 for false, 1 for true, 2 for random, for n+1 gods.
// But it is easier to use 4 bits per god, even though it potentially wastes space.
// The number of gods might not even be large enough for the wasted space to
// be an issue. Besides, 3 for a god might be used to stand for "unknown" in some 
// places. In fact, 3 for a god would never be used in a significant way.

typedef uint8_t God;  // The type of a god. 0=false, 1=true, 2=random.
// Uses 2 bits per god. 3 might be used for "unknown" in some places.
//   The second god will be in bits 2-3... Fourth in 6-7. With 25% waste.
//   For efficiency reasons, randomGod should probably have been 0,
// since that's what most tests are againts. It should, maybe, be safe to
// change random to 0, still.
#define God_Max (UINT8_MAX)
#define God_GodsPerSlot (4)

#define God_False ((God)0)
#define falseGod ((God)0)
#define God_True ((God)1)
#define trueGod ((God)1)
#define God_Random ((God)2)
#define randomGod ((God)2)


// The return values of functions.
//   0 (Hard_resultFound) also stands for normal result but where
// the path adding is postponed a little.
//   Hard_aborted stands for a heuristic result where we should
// abort the current search. 
#define Hard_outOfMemory 2
#define Hard_resultFound 0
#define Hard_aborted 1
#define Hard_pathAdded Hard_resultFound
#define Hard_pathPostponed 3


typedef uint8_t GodsN;  // The number of gods. Should maybe be larger.
#define GodsN_Undef (UINT8_MAX)


// The main structure containing the investigation settings and data.
typedef struct Hard_
{
    GodsN godsN;  // The number of gods.
    GodsN fGodsN; // The number of false gods.
    GodsN tGodsN; // The number of true gods.
    GodsN rGodsN; // The number of random gods.
    // godsN = fGodsN + tGodsN + rGodsN.
    // rGodsN < fGodsN + tGodsN.

    uint64_t possN;  // Number of possibilities for the gods.

    //GodsN conjSize;  // = godsN/4 + (godsN%4!=0). The size of a conjunction.
                       // In God slots (bytes).
    // Shouldn't be used. Use exact god numbers instead.
    //   With padding of conjunctions, so that they always start at the
    // start of a byte, conjSize could be useful.
    //   Padding of conjunctions is probably a good idea, for efficiency
    // reasons.

    uint64_t slotsUsed;  // Number of god slots filled up, in god memory below.

    uint8_t cellsUsed;  // Number of cells used in first non-filled god slot 
                        // below. 0-3.

    GodsN godsInConj;  // Number of gods in current conjunction being built.
                       //   You could derive this from slotsUsed and cellsUsed,
                       // and other values, probably.

    uint64_t godsSize;  // The size of gods below, in number of 
                        // bytes (God).
                        //   It's unclear how large this should be?
                        // We probably want it large enough to not have to
                        // check the size, and reallocate. But not much
                        // larger than necessary. For even splits, there
                        // should be some log extra space (n*log(n)) needed.
                        // However, uneven splits might move closer to 
                        // quadratic space. But maybe not, with pruning, with
                        // some decent first call. On top of that, in all cases, 
                        // there is extra space to handle the possibilities
                        // that the asked gods are random.
                        //   We'll probably go with some safe quadratic-ish
                        // size. It shouldn't matter too much.
                        //   And we'll check the bound and reallocate if needed.
                        // And then use relative pointers and not fixed ones.
                        //   In fact, let's not try to guess the size, and
                        // instead start with some not too large size, and then
                        // extend as needed.
                        //   Actually, only a fixed amount of memory is needed ---
                        // we'll resort disjuncts in local disjunctions instead.
                        // Only ->possN much is neaded, plus an extra poss0R0 that
                        // isn't really needed but due to initial setup.
                        //   Bound checks will be performed before conjunctions
                        // or disjunctions are entered.
    God * gods;  // Memory for disjunctions of conjunctions.
    
    ConjHash * ht;  // A hash table to store results.

    GodsN * qPath;  // The gods questioned so far, in a "stack",
                    // to be used to calculate number of randoms asked.
    //GodsN godsInqPath;  // The number of gods in qPath. Not used since
                          // it's the same as number of questions asked.
    uint16_t qPathSize;  // Size of qPath. In number of max gods.

    God * prefix;  // The conjuncts in a "stack", to be used as prefix.
    GodsN godsInPrefix;  // The number of gods in prefix.

    // A supplied upper bound, or the best result so far if that's lower.
    double upperBound;

    // These won't be accurate but used for heristics. E.g., probabilities
    // won't be taken into account, and subresultsFound can be > possN. However,
    // they are appropriate for the abort heuristics.
    //   Actually, this is a problem. Since low probabilities will get 
    // higher weights (that is number of questions), counting low
    // probabilites as full will bias the estimate. We'll try to 
    // weigh new additions according to the probabilities instead.
    // For example, before 2 + 3(1/2^1) got sum 5. Now it gets,
    // 2 + ((3/k) + (old average)*((k-1)/k), with k = 2^1, and with 3 as
    // new addition, with probability 1/k.
    //   Will this new approach bias the estimate in the other direction?
    // Because it *is* likely that weight is the correct weight --- you just
    // don't want to count it multiple times. Indeed, it will be biased in the
    // other direction. And you can't keep track of all the values involved
    // to fix it. Still, this might be better than the high bias from
    // counting high weights multiple times.
    //   Might it be better to add weights probabilistically? Add
    // full weight with probability 1/2^r? Add option flag to choose!?
    uint64_t subresultsFound;  // Number of sub-results found. For heuristics.
    double subresSum;  // The sum of the sub-results. (Now of type double.)

    // A search aborts when estimated result is > abortLeeway*upperBound.
    // abortLeeway = abortLeewayStart at the start.
    // abortLeeway is abortLeewayEnd towards the end, and never less.
    double abortLeeway;    

    // What abortLeeway is decremented by, after each disjunct found.
    double abortLeewayDecrement;

    // Catch this many more aborts, for the current search.
    uint64_t catchAbortsN;

    // Below, for retries and catches, we won't retry with no randoms.

    // Best estimated results for the positive sides, for each level.
    //   0 corresponds to the positive sides; 1 to negative sides.
    // We'll use some diagonalization coding:
    // +0: 0:0. +1: 1:00, 2:01. +3: 3:000, 4:001, 5:010, 6:011. ...
    // +2^depth-1: <call sequence for the depth>
    double * bestPositiveEstimates;  // [2^(maxCatchDepth+1)-1]

} Hard;



// Each bit in a verbosity vector decides if some particular info should be
// printed when appropriate. See below for what each bit means.
typedef uint32_t HardVerbosityVector;

#define HardVerbosity_printErrors  ( (HardVerbosityVector)1 << 0 )
#define HardVerbosity_printResult  ( (HardVerbosityVector)1 << 1 )
#define HardVerbosity_printInfo    ( (HardVerbosityVector)1 << 2 )
#define HardVerbosity_printMore    ( (HardVerbosityVector)1 << 3 )
#define HardVerbosity_printSeed    ( (HardVerbosityVector)1 << 4 )
#define HardVerbosity_printY       ( (HardVerbosityVector)1 << 5 )
#define HardVerbosity_printTime    ( (HardVerbosityVector)1 << 6 )
#define HardVerbosity_printAll     ( (HardVerbosityVector)1 << 7 )
#define HardVerbosity_printZ       ( (HardVerbosityVector)1 << 8 )


// Max support for some things when it comes to (nested) questions
// when there are still randoms.
#define MaxDepth 256

// Default repeated sub-tries.
#define DefaultReps 2

// Default repeats from this level and below will be 0.
#define ZeroRepsFromLvl 3



// Structure containing settings.
typedef struct Settings_
{
    // A bit vector where each bit determines if some particular info should
    // be printed. 0 means quiet.
    HardVerbosityVector verbosityVector;

    FILE * outFile;      // The file that the result is written to.
    char * outFileName;  // The name of outFile.

    uint8_t precision;   // The precision used when printing floats.
    
    double memIncFactor;  // What factor the ->gods memory is to be enlarged by
                          // when it needs to be reallocated. Not used.

    // The current seed. The default is time(NULL).
    unsigned long int seed;

    // The greater the value, the more swaps will be tried.
    uint8_t doSwaps;  // 0 means no conjunction swaps to try to get gi 
                      // and gj non-random. 
                      // 1 means only perfect swaps. 
                      // 2 means also upper non-perfect swaps. 
                      // 3 means also lower non-perfect swaps.
                      // 4 means also 0111 non-perfect swaps.
                      // 5 means also 1101 non-perfect swaps.
                      // 6 means also "non-improving" swaps, to balance 
                      // sides, but not for 0 random sides.
                      // 7 means also balance always. 

    // The greater the value, the harder we'll try to find promising
    // gods to pursue.
    uint8_t findGoodGods;
    // 0 means random (by chance) gods.
    // 1 means a simple, linear search
    // 2 means a quadratic search. 
    // 3 means 2 with added virtual perfect swaps, which could be cubicish. 

    // Do work to try to optimize case where a non-R god is found.
    uint8_t optimizeNonR;
    // 0 means assignment of floor logs in order, with no extra search.
    // 1 means that some promising disjuncts get floor logs before rest.
    // 2 means 1 and some additional work.
    // n means doing n optimization iterations.
    //   There should be little reason to not have a high value.

    // 0 means no shuffling.
    // 1 means only non-g0=R conjunctions are shuffled, once.
    // 2 means only g0=R conjunctions are shuffled, once.
    // 3 means all conjunctions are shuffled (within their partitions), once.
    // 4 means also non-gq=R conjunctions are shuffled at every recursive call.
    // 5 means 3 and that gq=R conjunctions are shuffled at every recursive call.
    // 6 means all of the above.
    uint8_t shuffleConjunctions;

    // 0 means no bias, random.
    // 1 means positive side will always have one more disjunct when
    //   there is an odd number of disjuncts.
    // 2 means negative side will always have one more disjunct when
    //   there is an odd number of disjuncts.
    // There are indications that positive sides have higher question
    // averages, indicating that option 2 might be good, to balance sides.
    uint8_t oddBias;

    // The search for a solution will be repeated this many times.
    uint32_t iterate;

    // A search aborts when estimated result is > abortLeeway*upperBound.
    // abortLeeway = abortLeewayStart at the start.
    double abortLeewayStart;
    
    // A search aborts when estimated result is > abortLeeway*upperBound.
    // abortLeeway is abortLeewayEnd towards the end, and never less.
    double abortLeewayEnd;

    // Don't abort until at least this many subresults are found.
    uint64_t dontAbortUntil;

    // When the result estimate at a node is < resumeAbortedLeeway *
    // upperBound, aborts will be caught and search resumed.
    double resumeAbortedLeeway;

    // Catch this many aborts.
    uint64_t catchAbortsN;

    // The maximal depth where aborts are caught. Starting at 0.
    uint8_t maxCatchDepth;

    // Take best result of this many tries (+1), for each level.
    uint16_t * lvlReps;  //[MaxDepth];

    // Indent this much times the question depth when printing.
    uint8_t indent;

    // The estimate heuristic used:
    // 0 means a probabilistic approach. This might be best?
    // 1 means an average weighing approach. This will be biased low.
    // 2 means counting weights with low probability fully, and repeatedly.
    //   This will have a high bias.
    uint8_t estimateHeuristic;

    // Best positive level 0 estimate. Only used to set the initial
    // bestPositiveEstimates[0], once.
    double bestLvl0PosEst;

    // If true, we'll use the upper bound globaly. If false,
    // we'll use local bounds.
    bool globalBound;

} Settings;



// The main structure containing everything.
typedef struct HardInstance_
{
    Hard * hard;

    Settings * settings;

} HardInstance;


// Returns a new HardInstance instance.
HardInstance * hard_newInstance(void);


// Allocates arrays. 
//   Also checks the number of different gods.
//   Returns true iff there wasn't enough memory.
bool hard_allocArrays( HardInstance * hi );


// Solves the generalization of "the hardest puzzle ever".
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_resultFound
// otherwise.
//   The algorithm will be derived from the paper
// 'How to Solve "The Hardest Logic Puzzle Ever" and Its
// Generalization'.
uint8_t hard_solve( HardInstance * hi );


// Frees and closes hi.
void hard_free( HardInstance * hi );


// Prints the conjunction c. c should point into hi->hard->gods.
void hard_printConjunction( uint64_t c, HardInstance * hi );


// The slot for the ath god.
#define godSlot(a)  ( (a) / God_GodsPerSlot )

// The cell for the ath god.
#define godCell(a)  ( (a) % God_GodsPerSlot )

// Returns the god value in the nth cell of g (starting at 0).
static inline God godVal( God g, uint8_t n )
{
    return ( g >> 2*n ) & (God)3;
}

// Returns the ith god value for g.
static inline God godV( God * g, uint64_t i )
{
    return godVal( g[godSlot(i)], godCell(i) );
}

// Sets the cell in slot in g to v.
//   Like setGodF.
#define setGod( g, slot, cell, v )                          \
    ( (g)[slot] = (g)[slot] & ~( (God)3 << (2*(cell)) )  |  \
                  ( (God)(v) << (2*(cell)) ) )


#endif // hard_H
