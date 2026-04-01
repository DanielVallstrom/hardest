// Module finding fewest questions that solve the generalization of
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
#include "common.h"
#include "compilerMacros.h"
#include "readBounds.h"


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <ctype.h>


// For threads.
//   However, OpenMP could slow down the program a lot, on things
// that should be to some extent embarrassingly parallelizable.
#ifdef OpenMP
//#include <omp.h>
#endif



// Returns a new HardInstance.
HardInstance * hard_newInstance(void)
{
    HardInstance * hi = malloc( sizeof(HardInstance) );

    if ( hi == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return NULL;
    }


    // Set up the settings part. ----------------------------

    hi->settings = malloc( sizeof(Settings) );

    if ( hi->settings == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return NULL;
    }

    Settings * s = hi->settings;

    s->verbosityVector = 127;
    s->verbosityVector |= HardVerbosity_printSeed;


    s->precision = DefaultPrecision;
    s->memIncFactor = 2.0;  // ??

    s->outFile = stdout;
    s->outFileName = NULL;

    s->seed = time(NULL);
    common_srand(s->seed);

    s->doSwaps = 7;  // ??
    s->findGoodGods = 5;  // ??
    s->optimizeNonR = 50;  // ??
    s->shuffleConjunctions = 7;  // ??
    s->iterate = 19;  // ??
    s->abortLeewayStart = 1.02;  // ??
    // End should maybe be smaller on easier problems. And larger on harder.
    s->abortLeewayEnd = 1.01;  // ??
    s->dontAbortUntil = 10;  // ??
    s->resumeAbortedLeeway = 1.01;  // ?? Good if >1.0, atm.
    s->catchAbortsN = 8;  // ??
    s->maxCatchDepth = 1;  // ??
    s->oddBias = 0;  // ??
    s->indent = 2;  // ??
    s->bestLvl0PosEst = DBL_MAX;
    s->estimateHeuristic = 0;  // ??
    s->globalBound = true;
    s->topLocalResetLevel = 2;  // ??
    s->goodGodsCandN = 250;  // ??
    s->maxUnbal = 2;  // ???  // UINT64_MAX;  // ??
    s->estWeight = 0.0;

    s->boundsFileName = "best_known_bounds.csv";
    s->backupBoundsFileName = "best_known_bounds.csv~";
    s->boundsFile = NULL;
    s->updateBoundsFile = true;  // ??
    s->useBoundsFileOptions = false;  // ??
    s->upperBoundInFile = DBL_MAX;  // Undefined.
    s->boundStatus = HardBoundStatus_undefined;
    s->useBoundFromFile = 1;  // ??
    s->printBoundUsed = true;  // ??
    s->noteReplications = 2;
    s->boundsFileSeed = UINT64_MAX;  // ?? undef. value?? (it's okayish)

    s->abortPromilleGoal = 960;  // ??
    s->abortLeewayChange = 0.001;  // ??
    s->changeFactor = 1.01;  // ??
    s->minSampleSize = 0;  // ??
    s->ciz = DefaultCIz;

    s->lvlReps = calloc( 256, sizeof(uint16_t) );

    if ( s->lvlReps == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return NULL;
    }

    for ( uint16_t n = 0; n != ZeroRepsFromLvl; n++ )
    {
        s->lvlReps[n] = DefaultReps;
    }

    // Open the bounds file. We'll re-open file if it's to be written to.
    // Or, let's wait with opening the file until it's needed.
    #if 0
    s->boundsFile = fopen( s->boundsFileName, "r" );

    if ( s->boundsFile == NULL )
    {
        fprintf( stderr, "\nCould not open best_known_bounds.csv\n\n" );
    }
    #endif

    // Set up the hard part. ------------------------------

    hi->hard = malloc( sizeof(Hard) );

    if ( hi->hard == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return NULL;
    }

    hi->hard->godsN  = GodsN_Undef;
    hi->hard->fGodsN = GodsN_Undef;
    hi->hard->tGodsN = GodsN_Undef;
    hi->hard->rGodsN = GodsN_Undef;

    hi->hard->cellsUsed = 0;
    hi->hard->slotsUsed = 0;
    hi->hard->godsInConj = 0;
    hi->hard->godsInPrefix = 0;
    hi->hard->qPathSize = DefaultqPathSize;
    hi->hard->upperBound = DBL_MAX;
    hi->hard->subresultsFound = 0;
    hi->hard->subresSum = 0;
    hi->hard->abortLeeway = s->abortLeewayStart;
    hi->hard->catchAbortsN = s->catchAbortsN;

    hi->hard->gods = NULL;
    hi->hard->prefix = NULL;
    hi->hard->qPath = NULL;
    hi->hard->ht = NULL;
    hi->hard->gGCandsPos = NULL;
    hi->hard->gGCandsNeg = NULL;

    return hi;
}



// Returns the number of possible god configurations.
//   n is total number of gods, f is false gods, t is true gods.
//   Overflows with n around 20 and greater.
static uint64_t poss( GodsN n, GodsN f, GodsN t )
{
    // Calculate number of possibilities, k, for the gods.
    uint64_t k = 1;
    
    // False gods:
    for ( uint64_t i = 0; i != f; i++ )
    {
        k *= n - i;
    }
    
    // The order doesn't matter.
    k = k / fac(f);

    // True gods:
    for ( uint64_t i = 0; i != t; i++ )
    {
        k *= n - f - i;
    }
    
    // The order doesn't matter.
    k = k / fac(t);

    return k;
}


// Returns the number of possible god configurations.
//   n is total number of gods, f is false gods, t is true gods.
//   Like poss but divides early to avoid overflow. And multiplies
// bottom up, instead of other way around.
// (Already 21!, or so, doesn't fit uint64_t.)
static uint64_t possSafe( GodsN n, GodsN f, GodsN t )
{
    // Calculate number of possibilities, k, for the gods.
    uint64_t k = 1;
    
    // False gods:
    if ( f <= 2 )
    {
        for ( uint64_t i = 0; i != f; i++ )
        {
            k *= n - i;
        }
        
        // The order doesn't matter.
        k = k / fac(f);
    }
    else
    {
        k = k * ( n - (f-0-1) );  // k * n

        for ( uint64_t i = 1; i != f; i++ )
        {
            k = k * (n-(f-i-1)) / (i+1);  // k * (n-i) / ...
        }
    }


    // True gods:
    if ( t <= 2 )
    {
        for ( uint64_t i = 0; i != t; i++ )
        {
            k *= n - f - i;
        }
    
        // The order doesn't matter.
        k = k / fac(t);
    }
    else
    {
        k = k * ( n - f - (t-0-1) );  // k * (n-f)

        for ( uint64_t i = 1; i != t; i++ )
        {
            k = k * ( n - f - (t-i-1) ) / (i+1);  // k * (n-f-i) / ...
        }
    }


    assert3( n <= 20 ? k == poss( n, f, t ) : true );


    return k;
}



#define bytesNeededForNGods(n)  \
    ( (n) / God_GodsPerSlot  +  ( (n) % God_GodsPerSlot != 0 ) )



// Returns an appropriate size for the gods memory, in number of 
// bytes (God).
//   It's unclear what this size should be though!!!???vvv
// From hard.h:
// "We probably want it large enough to not have to
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
// extend as needed."
//   Actually, only a fixed amount of memory is needed ---
// we'll resort disjuncts in local disjunctions instead.
// Only ->possN much is neaded, plus an extra poss0R0 that
// isn't really needed but due to initial setup.
//   poss0R0 is the initial number of possibilities when
// the first god is random.
static uint64_t godsSize( Hard * h, uint64_t poss0R0 )
{
    return bytesNeededForNGods( ( h->possN + poss0R0 ) * h->godsN );
    
    // + 1 + h->godsN / God_GodsPerSlot + 1;
    // + 1 + h->godsN... shouldn't be needed, but doesn't matter.

    /*
    uint64_t b =  h->possN / 2 + poss0R0/2;

    uint64_t c = (uint64_t)sqrt(b) + 2;

    //uint64_t s = b * max( b/12, c );
    uint64_t s = b * c;

    return (s + 20) * h->conjSize;
    */
}



// Allocates Hard arrays.
//   Also checks and sets the number of different gods.
//   Calculates possN too.
//   Returns true iff there wasn't enough memory, or if god sanity 
// check failed.
//   Also prints info.
//   Also handles the bounds file, closes it.
//   Also handles minSampleSize, if 0.
bool hard_allocArrays( HardInstance * hi )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;
 

    if ( 1 < ( ( h->godsN  == GodsN_Undef ) + ( h->fGodsN == GodsN_Undef ) +
               ( h->tGodsN == GodsN_Undef ) + ( h->rGodsN == GodsN_Undef ) ) )
    {
        fprintf ( stderr, 
                  "\nError: number of gods is undefined.\n\n" );

        return true;
    }

    if ( h->godsN == GodsN_Undef )
    {
        h->godsN = h->fGodsN + h->tGodsN + h->rGodsN;
    }
    else if ( h->fGodsN == GodsN_Undef )
    {
        h->fGodsN = h->godsN - h->tGodsN - h->rGodsN;
    }
    else if ( h->tGodsN == GodsN_Undef )
    {
        h->tGodsN = h->godsN - h->fGodsN - h->rGodsN;
    }
    else if ( h->rGodsN == GodsN_Undef )
    {
        h->rGodsN = h->godsN - h->fGodsN - h->tGodsN;
    }

    if ( h->godsN != h->fGodsN + h->tGodsN + h->rGodsN )
    {
        fprintf ( stderr, 
                  "\nError: number of gods don't add up.\n\n" );

        return true;
    }

    if ( h->rGodsN >= h->fGodsN + h->tGodsN )
    {
        fprintf ( stderr, 
                  "\nUnsolvable: random gods >= non-random gods.\n\n" );

        return true;
    }

    if ( h->rGodsN == 0 )
    {
        fprintf ( stderr, "\nAborting: no random gods.\n\n" );

        return true;
    }


    // Set conjuction sizes. In number of God slots (bytes).
    //h->conjSize = h->godsN / God_GodsPerSlot + 
    //              ( h->godsN % God_GodsPerSlot != 0 );


    GodsN  godsN = h->godsN;
    GodsN fGodsN = h->fGodsN;
    GodsN tGodsN = h->tGodsN;
    //GodsN rGodsN = h->rGodsN;


    s->goodGodsCandN = min( s->goodGodsCandN, godsN );
    

    h->possN = possSafe( godsN, fGodsN, tGodsN );

    #if AssertionLevel >= 4
    for ( uint8_t i = 2; i != 20; i+=2 )
    {
        assert4( poss( i+1, 0, i   )  ==  possSafe( i+1, 0, i ) );
        assert4( poss( i+2, 0, i+1 )  ==  possSafe( i+2, 0, i+1 ) );
        assert4( poss( i+1, 1, i-1 )  ==  possSafe( i+1, 1, i-1 ) );
        assert4( poss( i+2, 2, i-1 )  ==  possSafe( i+2, 2, i-1 ) );
    }
    #endif
    
    if ( s->verbosityVector & HardVerbosity_printMore )
    {
        fprintf( s->outFile,
                 "|gods|=%u |fGods|=%u |tGods|=%u |rGods|=%u \n",
                 (unsigned)godsN, (unsigned)fGodsN, (unsigned)tGodsN,
                 (unsigned)h->rGodsN );
    }

    if ( s->verbosityVector & HardVerbosity_printInfo )
    {
        fprintf( s->outFile,
                 "possibilities: %llu\n",
                 (unsigned long long)h->possN );
    }


    // Read and close the bounds file.
    
    s->boundsFile = fopen( s->boundsFileName, "r" );

    if ( s->boundsFile == NULL )
    {
        fprintf( stderr, "\nCould not open best_known_bounds.csv\n\n" );
    }
    else
    {
        readBounds_readFile(hi);

        fclose(s->boundsFile);
    }


    // The gods array should be big array of question matrices memory,
    // for recursive calls.

    // The initial number of god possibilities given g0=R.
    uint64_t poss0R0 = possSafe( godsN-1, fGodsN, tGodsN );

    h->godsSize = godsSize( h, poss0R0 );

    h->gods = malloc( h->godsSize * sizeof(God) );

    h->prefix = malloc( bytesNeededForNGods(godsN) );

    h->qPath = malloc( h->qPathSize * sizeof(GodsN) );

    h->ht = conjHash_new( h );

    h->bestPositiveEstimates = malloc( s->maxCatchDepth * sizeof(double) );

    h->gGCandsPos = malloc( 2 * s->goodGodsCandN * sizeof(God) );
    h->gGCandsNeg = malloc( 2 * s->goodGodsCandN * sizeof(God) );

    if ( h->gods == NULL  ||  h->prefix == NULL  ||  h->qPath == NULL  ||
         h->ht == NULL  ||  h->bestPositiveEstimates == NULL  ||
         h->gGCandsPos == NULL  ||  h->gGCandsNeg == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return true;
    }

    h->bestPositiveEstimates[0] = s->bestLvl0PosEst;
    for ( uint8_t n = 1; n != s->maxCatchDepth; n++ )
    {
        h->bestPositiveEstimates[n] = DBL_MAX;
    }

    // Not used.
    //memset( h->gods, God_Max, sizeof(God) * h->godsSize );


    // Set abort leeway decrements.
    h->abortLeewayDecrement = ( s->abortLeewayStart - s->abortLeewayEnd ) /
                              h->possN * 1.000000001;  // ??


    h->abortLeeway = s->abortLeewayStart;
    h->catchAbortsN = s->catchAbortsN;
      

    // If the minimal sample size for updating abort heuristics is 0,
    // then we'll try to set it to something reasonable, conservatively.
    //   Depending on iterations, abort-goal.
    if ( s->minSampleSize == 0 )
    {
        // This sample size might be roughly suitable.
        double size = s->abortPromilleGoal >= 500 ? 
                      5 / ( 1 - ( s->abortPromilleGoal / 1000.0 ) ) : 
                      5 / ( s->abortPromilleGoal / 1000.0 );                    

        size /= 2;

        if ( s->iterate > 20000 )
        {
            size *= 2;
        }
        else
        {
            size += size * ( (double)s->iterate / 20000 ); 
        }

        s->minSampleSize = size;
    }

    return false;
}



// Returns true iff the god in the first cell of g is false.
static inline bool isFalse0( God g )
{
    return !( g & (God)3 );
}

// Returns true iff the god in the first cell of g is true.
//   3 for a god will not be used here.
static inline bool isTrue0( God g )
{
    return g & trueGod;
}

// Returns true iff the god in the first cell of g is random.
static inline bool isRandom0( God g )
{
    return g & randomGod;
}


// Returns true iff the god in the nth cell of g (starting at 0) is false.
static inline bool isFalseN( God g, uint8_t n )
{
    return !( ( g >> 2*n ) & (God)3 );
}

// Returns true iff the god in the nth cell of g (starting at 0) is true.
//   3 for a god will not be used here.
static inline bool isTrue( God g, uint8_t n )
{
    return ( g >> 2*n ) & trueGod;
}

// Returns true iff the god in the nth cell of g (starting at 0) is random.
static inline bool isRandom( God g, uint8_t n )
{
    return ( g >> 2*n ) & randomGod;
}


// Returns the god value in the first cell of g.
static inline God godVal0( God g )
{
    return g & (God)3;
}


// These are defined in the header file.
# if 0

// Returns the god value in the nth cell of g (starting at 0).
static inline God godVal( God g, uint8_t n )
{
    return ( g >> 2*n ) & (God)3;
}



// The slot for the ath god.
#define godSlot(a)  ( (a) / God_GodsPerSlot )

// The cell for the ath god.
#define godCell(a)  ( (a) % God_GodsPerSlot )



// Returns the ith god value for g.
static inline God godV( God * g, uint64_t i )
{
    return godVal( g[godSlot(i)], godCell(i) );
}

#endif



#define last3GodsMask ( (God)0b11111100 )
#define last2GodsMask ( (God)0b11110000 )
#define last1GodsMask ( (God)0b11000000 )

#define fst1GodsMask  ( (God)0b00000011 )
#define fst2GodsMask  ( (God)0b00001111 )
#define fst3GodsMask  ( (God)0b00111111 )


// Returns the first byte of a conjunction, with bits not in the
// conjunction zeroed out.
//   i is the start (in cells). g is h->gods.
//   If there are only 3 gods, starting at a byte,
// this won't work; fst3GodsMask would need to be used then.
static inline God fstByte( uint64_t i, God * g )
{
    switch ( i % God_GodsPerSlot )
    {
    case 0:
        return g[godSlot(i)];
        break;
    
    case 1:
        return g[godSlot(i)] & last3GodsMask;
        break;

    case 2:
        return g[godSlot(i)] & last2GodsMask;
        break;

    case 3:
        return g[godSlot(i)] & last1GodsMask;
        break;
        
    default:
        // This can't happen.
        break;
    }
}


// Returns the first byte of a conjunction, with bits not in the
// conjunction zeroed out.
//   i is the start (in cells). g is h->gods. n is number of gods.
//   If there are only 3 gods, this version is safe to use.
static inline God fstByte3( uint64_t i, God * g, GodsN n )
{
    switch ( i % God_GodsPerSlot )
    {
    case 0:
        if Unlikely ( n == 3 )
        { 
            return g[godSlot(i)] & fst3GodsMask;
        }
        else
        {
            return g[godSlot(i)];
        }

        break;
    
    case 1:
        return g[godSlot(i)] & last3GodsMask;
        break;

    case 2:
        return g[godSlot(i)] & last2GodsMask;
        break;

    case 3:
        return g[godSlot(i)] & last1GodsMask;
        break;
        
    default:
        // This can't happen.
        break;
    }
}


// Returns the last byte of a conjunction, with bits not in the
// conjunction zeroed out.
//   i is the start (in cells). g is h->gods. n is number of gods.
static inline God lastByte( uint64_t i, God * g, GodsN n )
{
    switch ( ( i + n ) % God_GodsPerSlot )
    {
    case 0:
        return g[godSlot(i+n-1)];
        break;
    
    case 1:
        return g[godSlot(i+n)] & fst1GodsMask;
        break;

    case 2:
        return g[godSlot(i+n)] & fst2GodsMask;
        break;

    case 3:
        return g[godSlot(i+n)] & fst3GodsMask;
        break;
        
    default:
        // This can't happen.
        break;
    }
}



// Sets the cell in slot in g to v.
static inline void setGodF( God * g, uint64_t slot,
                            uint8_t cell, God v )
{
    // The bits in question are first cleared (&), then set (|).
    g[slot] = g[slot] & ~( (God)3 << (2*cell) )  |  ( (God)v << (2*cell) );
}


#if 0
// Sets the cell in slot in g to v.
//   Like setGodF.
#define setGod( g, slot, cell, v )                          \
    ( (g)[slot] = (g)[slot] & ~( (God)3 << (2*(cell)) )  |  \
                  ( (God)(v) << (2*(cell)) ) )
#endif


// Sets the first free cell in g to v.
//   Also increments counters.
#define setGodInc( g, slotsUsed, cellsUsed, v )                  \
    do                                                           \
    {                                                            \
        (g)[slotsUsed] =                                         \
            (g)[slotsUsed] & ~( (God)3 << (2*(cellsUsed)) )  |   \
            ( (God)(v) << (2*(cellsUsed)) );                     \
                                                                 \
        (cellsUsed) = ( (cellsUsed) + 1 ) % God_GodsPerSlot;     \
                                                                 \
        if ( (cellsUsed) == 0 )  {(slotsUsed)++;}                \
    }                                                            \
    while (0)


// Like setGodInc.
#define setGodIncH( h, v )                                               \
    do                                                                   \
    {                                                                    \
        ((h)->gods)[(h)->slotsUsed] =                                    \
            ((h)->gods)[(h)->slotsUsed] &                                \
            ~( (God)3 << (2*((h)->cellsUsed)) )  |                       \
            ( (God)(v) << (2*((h)->cellsUsed)) );                        \
                                                                         \
        ((h)->cellsUsed) = ( ((h)->cellsUsed) + 1 ) % God_GodsPerSlot;   \
                                                                         \
        if ( ((h)->cellsUsed) == 0 )  {((h)->slotsUsed)++;}              \
    }                                                                    \
    while (0)



// Copies gods in prefix to ->gods.
static inline void copyGods( Hard * h )
{
    for ( GodsN i = 0; i != h->godsInPrefix; i++ )
    {
        setGodIncH( h, godVal( h->prefix[godSlot(i)], godCell(i) ) );
    }
}
    


// Sets the first free cell to v.
//   Updates cellsUsed and slotsUsed, and godsInConj.
//   If a new conjunction is to be started, the prefix is added.
static inline void setGodCP( Hard * h, God v )
{
    // See if a new conjunction is to begin.
    if Unlikely( h->godsInConj == h->godsN )
    {
        // Copy prefix to the new conjunction.
        copyGods( h );
        h->godsInConj = h->godsInPrefix;
    }

    setGodIncH( h, v );
    h->godsInConj++;
}



// Adds v to the prefix. Updates counter.
static inline void addToPrefix( Hard * h, God v )
{
    // The bits in question are first cleared (&), then set (|).
    h->prefix[ godSlot( h->godsInPrefix ) ] = 
        h->prefix[ godSlot( h->godsInPrefix ) ] & 
        ~( (God)3 << ( 2 * godCell( h->godsInPrefix ) ) )  |
        ( (God)v << ( 2 * godCell( h->godsInPrefix ) ) );

    h->godsInPrefix++;
}



// Set all god possibilities. 
//   Tree prefix stack is stored in h->prefix.
//   It's important that possibilities starting with R are all last,
// which they are.
static void setPoss( GodsN f, GodsN t, GodsN r, Hard * h )
{
    // Set first free god to F and continue recursively.
    if ( f != 0 )
    {
        setGodCP( h, falseGod );

        addToPrefix( h, falseGod );

        setPoss( f-1, t, r, h );

        h->godsInPrefix--;
    }


    // Set first free god to T and continue recursively.
    if ( t != 0 )
    {
        setGodCP( h, trueGod );

        addToPrefix( h, trueGod );

        setPoss( f, t-1, r, h );

        h->godsInPrefix--;
    }


    // Set first free god to R and continue recursively.
    if ( r != 0 )
    {
        setGodCP( h, randomGod );

        addToPrefix( h, randomGod );
        
        setPoss( f, t, r-1, h );

        h->godsInPrefix--;
    }
}



// Prints the god value v.
static void printGod( God v, FILE * s )
{
    switch ( v )
    {
    case falseGod:
        putc( 'F', s );

        break;

    case trueGod:
        putc( 'T', s );

        break;

    case randomGod:
        putc( 'R', s );

        break;
        
    default:
        fprintf( stderr, "\nError: god is not F, T, or R.\n\n" );

        exit(EXIT_FAILURE);

        break;
    }
}



// Prints the conjunction c. c should point into hi->hard->gods.
void hard_printConjunction( uint64_t c, HardInstance * hi )
{
    Hard * h = hi->hard;
    God * g = h->gods;
    Settings * s = hi->settings;

    for ( uint64_t cEnd = c + h->godsN; c != cEnd; c++ )
    {
        printGod( godVal( g[godSlot(c)], godCell(c) ), s->outFile );
    }

    fputc( '\n', s->outFile );
}



// Checks if it's safe to add n gods to ->gods memory. 
// If not, memory is reallocated.
//   Returns true iff not enough memory could be allocated.
static bool checkMemoryBound( HardInstance * hi, uint64_t n )
{
    Hard * h = hi->hard;

    if Unlikely( ( h->godsSize - h->slotsUsed ) * God_GodsPerSlot - 
                 h->cellsUsed  <  n )
    {
        uint64_t newGodsSize = 
            max( (uint64_t)( h->godsSize * hi->settings->memIncFactor ),
                 h->slotsUsed + n/God_GodsPerSlot + 100 );
        God * newGods = realloc( h->gods, newGodsSize * sizeof(God) );

        if ( newGods == NULL )
        {
            fprintf( stderr, "\nError: not enough memory.\n\n" );

            return true;
        }

        h->gods = newGods;
        h->godsSize = newGodsSize;

        // Print info.
        if ( hi->settings->verbosityVector & HardVerbosity_printAll )
        {
            fprintf( stdout, "Reallocated. This won't happen, now.\n" );
        }
    }

    return false;
}



// Swap conjunctions starting at the ith and jth gods.
//   Memory (i and j) might very well be unaligned. You could maybe use padding
// to align e.g. conjunctions, and maybe make e.g. this swap faster.
// Overall, this unpadded way might be best, and easiest, still.
static inline void swapConjs( Hard * h, uint64_t i, uint64_t j )
{
    God * g = h->gods;

    for ( GodsN k = 0; k != h->godsN; k++ )
    {
        God ig = godVal( g[godSlot(i+k)], godCell(i+k) );
        God jg = godVal( g[godSlot(j+k)], godCell(j+k) );

        setGod( g, godSlot(i+k), godCell(i+k), jg );
        setGod( g, godSlot(j+k), godCell(j+k), ig );
    }
}



#define cellsUsedTot(h)  ( (h)->slotsUsed * God_GodsPerSlot + (h)->cellsUsed )



// Counts randoms in n conjuctions, at ith conjunct, and starting at i.
static inline uint64_t countRs( Hard * h, uint64_t i, uint64_t n )
{
    God * g = h->gods;
    GodsN godsN = h->godsN;

    uint64_t rs = 0;  // Number of random gods in position.

    for ( uint64_t k = 0; k != n; k++ )
    {
        if ( godV( g, i + k*godsN )  ==  randomGod )
        {
            rs++;
        }
    }

    return rs;
}



// Not used.
#if 0
// Counts randoms in conjuctions starting at start. 
//   rs is the number of special conjunctions at the top, where gq=R.
//   posN is the number of conjunctions in the positive half, not counting
// the rs block. negN is the number for the negative half. posN and negN might
// be 0.
//   Results for most promising gods will be placed in gi, giRs, gj, and gjRs.
// giRs and gjRs will include randoms in the rs block.
// God gq is skipped.
static inline void 
count2RsSkipgq( HardInstance * hi, uint64_t start, uint64_t rs,
                uint64_t posN, uint64_t negN,
                GodsN * gi, uint64_t * giRs, 
                GodsN * gj, uint64_t * gjRs, GodsN gq )
{
    Hard * h = hi->hard;
    //God * g = h->gods;
    GodsN n = h->godsN;
    Settings * s = hi->settings;


    if ( s->findGoodGods == 0 )
    {
        // Pick i and j != gq at random. n-1 will correspond to gq. gq+1 to 0,...

        uint64_t k = common_randomNBiased(n-1);
        k = ( k + gq + 1 ) % n;  // Shift random so that n-1 maps to gq.
        assert( k != gq  &&  k < n );
        *gi = k;

        k = common_randomNBiased(n-1);  // One could repeat until different from i case.
        k = ( k + gq + 1 ) % n;  // Shift random so that n-1 maps to gq.
        assert( k != gq  &&  k < n );
        *gj = k;

        // Count random gods for i and j.
        *giRs  = countRs( h, start + rs * n +            *gi, posN );
        *gjRs  = countRs( h, start + rs * n + posN * n + *gj, negN );
        *giRs += countRs( h, start + *gi, rs );
        *gjRs += countRs( h, start + *gj, rs );

        return;
    }


    if ( s->findGoodGods == 1      || /* temp disjunct */ s->findGoodGods > 1 )
    {
        // Do the i side.

        God bestiGod;  // Best god found so far.
        God sndBestiGod;  // Second best god so far. Not used atm.
        uint64_t bestiGodRs = UINT64_MAX;  // Number of Rs for bestiGod.
        uint64_t sndBestiGodRs = UINT64_MAX;  // Number of Rs for sndBestiGod.

        for ( God i = 0; i != n; i++ )
        {
            if ( i == gq )
            {
                continue;
            }

            uint64_t iRs = countRs( h, start + rs*n + i, posN );

            iRs += countRs( h, start+i, rs );

            if ( iRs < sndBestiGodRs )
            {
                if ( iRs < bestiGodRs )
                {
                    sndBestiGod = bestiGod;
                    sndBestiGodRs = bestiGodRs;
                    bestiGod = i;
                    bestiGodRs = iRs;
                }
                else
                {
                    sndBestiGod = i;
                    sndBestiGodRs = iRs;
                }
            }
        }


        // Do the j side.

        God bestjGod;  // Best god found so far.
        God sndBestjGod;  // Second best god so far. Not used atm.
        uint64_t bestjGodRs = UINT64_MAX;  // Number of Rs for bestjGod.
        uint64_t sndBestjGodRs = UINT64_MAX;  // Number of Rs for sndBestjGod.

        for ( God j = 0; j != n; j++ )
        {
            if ( j == gq )
            {
                continue;
            }

            uint64_t jRs = countRs( h, start + rs*n + posN*n + j, negN );

            jRs += countRs( h, start+j, rs );

            if ( jRs < sndBestjGodRs )
            {
                if ( jRs < bestjGodRs )
                {
                    sndBestjGod = bestjGod;
                    sndBestjGodRs = bestjGodRs;
                    bestjGod = j;
                    bestjGodRs = jRs;
                }
                else
                {
                    sndBestjGod = j;
                    sndBestjGodRs = jRs;
                }
            }
        }

        *gi = bestiGod;
        *giRs = bestiGodRs;
        *gj = bestjGod;
        *gjRs = bestjGodRs;
    }


// todo vvv!!!

}
#endif



// Tries to swap conjunctions of two blocks, starting at iStart and jStart,
// with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at 1 in the i block, and at 2 in
// the j block.
//   The swaps will be "perfect".
static void swapConjunctions( Hard * h, uint64_t iStart, uint64_t iN,
                                        uint64_t jStart, uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + 1 + i*n;
    uint64_t jj = jStart + 2 + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii   ) != randomGod ||
                               godV( g, ii+1 ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj   ) != randomGod ||
                                   godV( g, jj-1 ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, starting at iStart and jStart,
// with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at 1 in the i block, and at 2 in
// the j block.
//   The swaps will be non-perfect "3" swaps.
static void swapConjunctions3a( Hard * h, uint64_t iStart, uint64_t iN,
                                          uint64_t jStart, uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + 1 + i*n;
    uint64_t jj = jStart + 2 + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii   ) != randomGod /* ||
                               godV( g, ii+1 ) == randomGod */ );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj   ) != randomGod ||
                                   godV( g, jj-1 ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, starting at iStart and jStart,
// with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at 1 in the i block, and at 2 in
// the j block.
//   The swaps will be non-perfect "3" swaps.
static void swapConjunctions3b( Hard * h, uint64_t iStart, uint64_t iN,
                                          uint64_t jStart, uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + 1 + i*n;
    uint64_t jj = jStart + 2 + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii   ) != randomGod ||
                               godV( g, ii+1 ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj   ) != randomGod /* ||
                                   godV( g, jj-1 ) == randomGod */ );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, starting at iStart and jStart,
// with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at 1 in the i block, and at 2 in
// the j block.
//   The swaps will be non-improving "2" swaps, but they will balance
// the sides. The lower, b side will be favored (because it is supposed
// to have more randoms in g2).
//   No more than maxSwaps swaps will be made.
static void swapConjunctions2b( Hard * h, uint64_t iStart, uint64_t iN,
                                          uint64_t jStart, uint64_t jN,
                                          uint64_t maxSwaps )
{
    if ( maxSwaps == 0 )
    {
        return;
    }

    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + 1 + i*n;
    uint64_t jj = jStart + 2 + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( /* godV( g, ii   ) != randomGod || */
                               godV( g, ii+1 ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj   ) != randomGod /* ||
                                   godV( g, jj-1 ) == randomGod */ );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;

                maxSwaps--;

                if ( maxSwaps == 0 )
                {
                    return;
                }
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, starting at iStart and jStart,
// with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at 1 in the i block, and at 2 in
// the j block.
// The swaps will be non-improving "2" swaps, but they will balance
// the sides. The upper, a side will be favored (because it is supposed
// to have more randoms in g1).
//   No more than maxSwaps swaps will be made.
static void swapConjunctions2a( Hard * h, uint64_t iStart, uint64_t iN,
                                          uint64_t jStart, uint64_t jN,
                                          uint64_t maxSwaps )
{
    if ( maxSwaps == 0 )
    {
        return;
    }

    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + 1 + i*n;
    uint64_t jj = jStart + 2 + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii   ) != randomGod /* ||
                               godV( g, ii+1 ) == randomGod */ );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( /* godV( g, jj   ) != randomGod || */
                                   godV( g, jj-1 ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;

                maxSwaps--;

                if ( maxSwaps == 0 )
                {
                    return;
                }
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be "perfect".
//   Like swapConjunctions, but for arbitrary gi and gj.
//   gi might equal gj, and then nothing happens.
static void swapConjunctionsG( Hard * h, GodsN gi, uint64_t iStart,
                                         uint64_t iN,
                                         GodsN gj, uint64_t jStart,
                                         uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii+gi ) != randomGod ||
                               godV( g, ii+gj ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj+gj ) != randomGod ||
                                   godV( g, jj+gi ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-perfect "3" swaps.
//   Like swapConjunctions3a, but for arbitrary gi and gj.
//   gi might equal gj, and then nothing happens.
static void swapConjunctionsG3a( Hard * h, GodsN gi, uint64_t iStart,
                                           uint64_t iN,
                                           GodsN gj, uint64_t jStart,
                                           uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii+gi ) != randomGod /* ||
                               godV( g, ii+gj ) == randomGod */ );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj+gj ) != randomGod ||
                                   godV( g, jj+gi ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-perfect "3" swaps.
//   Like swapConjunctions3b, but for arbitrary gi and gj.
//   gi might equal gj, and then nothing happens.
static void swapConjunctionsG3b( Hard * h, GodsN gi, uint64_t iStart,
                                           uint64_t iN,
                                           GodsN gj, uint64_t jStart,
                                           uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii+gi ) != randomGod ||
                               godV( g, ii+gj ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj+gj ) != randomGod /* ||
                                   godV( g, jj+gi ) == randomGod */ );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-perfect "3" swaps.
//   gi might equal gj, and then nothing happens.
static void swapConjunctions0111( Hard * h, GodsN gi, uint64_t iStart,
                                            uint64_t iN,
                                            GodsN gj, uint64_t jStart,
                                            uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( /* godV( g, ii+gi ) != randomGod || */
                               godV( g, ii+gj ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj+gj ) != randomGod ||
                                   godV( g, jj+gi ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-perfect "3" swaps.
//   gi might equal gj, and then nothing happens.
static void swapConjunctions1101( Hard * h, GodsN gi, uint64_t iStart,
                                            uint64_t iN,
                                            GodsN gj, uint64_t jStart,
                                            uint64_t jN )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii+gi ) != randomGod ||
                               godV( g, ii+gj ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( /* godV( g, jj+gj ) != randomGod || */
                                   godV( g, jj+gi ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-improving "2" swaps, but they will balance
// the sides. The lower, b (j) side will be favored (because it is supposed
// to have more randoms in gj, or because we are trying to get it to 0 randoms).
//   No more than maxSwaps swaps will be made.
//   Like swapConjunctions2b, but for arbitrary gi and gj.
static void swapConjunctionsG2b( Hard * h, /* GodsN gi, */ uint64_t iStart,
                                           uint64_t iN,
                                           GodsN gj, uint64_t jStart,
                                           uint64_t jN,
                                           uint64_t maxSwaps )
{
    if ( maxSwaps == 0 )
    {
        return;
    }

    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( /* godV( g, ii+gi ) != randomGod || */
                               godV( g, ii+gj ) == randomGod );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( godV( g, jj+gj ) != randomGod /* ||
                                   godV( g, jj+gi ) == randomGod */ );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;

                maxSwaps--;

                if ( maxSwaps == 0 )
                {
                    return;
                }
            }
        }
    }
}



// Tries to swap conjunctions of two blocks, i and j, starting at iStart
// and jStart, with iN and jN the number of conjunctions in the blocks,
// with the aim of minimizing randoms at gi in the i block, and at gj in
// the j block.
//   The swaps will be non-improving "2" swaps, but they will balance
// the sides. The upper, a (i) side will be favored (because it is supposed
// to have more randoms in gi, or because we are trying to get it to 0 randoms).
//   No more than maxSwaps swaps will be made.
//   Like swapConjunctions2a, but for arbitrary gi and gj.
static void swapConjunctionsG2a( Hard * h, GodsN gi, uint64_t iStart,
                                           uint64_t iN,
                                           /* GodsN gj, */ uint64_t jStart,
                                           uint64_t jN,
                                           uint64_t maxSwaps )
{
    if ( maxSwaps == 0 )
    {
        return;
    }

    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t i = 0; 
    uint64_t j = 0; 

    uint64_t ii = iStart + i*n;
    uint64_t jj = jStart + j*n;

    while ( i != iN  &&  j != jN )
    {
        // Find i conjunction to swap.
        for ( ; i != iN  &&  ( godV( g, ii+gi ) != randomGod /* ||
                               godV( g, ii+gj ) == randomGod */ );
              i++, ii+=n )
        {
        }

        if ( i != iN )
        {
            // Find j conjuction to swap.
            for ( ; j != jN  &&  ( /* godV( g, jj+gj ) != randomGod || */
                                   godV( g, jj+gi ) == randomGod );
                  j++, jj+=n )
            {
            }

            if ( j != jN )
            {
                swapConjs( h, iStart + i*n, jStart + j*n );

                i++;
                ii+=n;
                j++;
                jj+=n;

                maxSwaps--;

                if ( maxSwaps == 0 )
                {
                    return;
                }
            }
        }
    }
}



// Counts randoms in conjuctions starting at start. 
//   rs is the number of special conjunctions at the top, where gq=R.
//   posN is the number of conjunctions in the positive half, not counting
// the rs block. negN is the number for the negative half. posN and negN might
// be 0.
//   Results for most promising gods will be placed in gi, giRs, gj, and gjRs.
// giRs and gjRs will include randoms in the rs block.
//   God gq is not skipped.
//   s->goodGodsCandN gods for each case will be considered, pairwise.
static void 
count2RsQuad( HardInstance * hi, uint64_t start, uint64_t rs,
                uint64_t posN, uint64_t negN,
                GodsN * gi, uint64_t * giRs, 
                GodsN * gj, uint64_t * gjRs /*, GodsN gq */ )
{
    Hard * h = hi->hard;
    GodsN n = h->godsN;
    Settings * s = hi->settings;
    GodsN candN = s->goodGodsCandN;
    God * ggPos = h->gGCandsPos;
    God * ggNeg = h->gGCandsNeg;

    God worstCandIdx;  // The position for worst candidate in ggx.
    uint64_t worstCandRs = 0;  // Number of Rs for worstCand.

    
    // Do the i side.

    // These are guaranteed to be preliminary candidates.
    for ( God i = 0; i != candN; i++ )
    {
        uint64_t iRs = countRs( h, start + rs*n + i, posN );

        iRs += countRs( h, start+i, rs );

        ggPos[2*i] = i;
        ggPos[2*i+1] = iRs;

        if ( iRs > worstCandRs )
        {
            worstCandRs = iRs;
            worstCandIdx = 2*i;
        }
    }

    for ( God i = candN; i != n; i++ )
    {
        uint64_t iRs = countRs( h, start + rs*n + i, posN );

        iRs += countRs( h, start+i, rs );

        if ( iRs < worstCandRs )
        {
            ggPos[worstCandIdx] = i;
            ggPos[worstCandIdx+1] = iRs;

            // Find new worst.
            worstCandIdx = 0;
            worstCandRs = ggPos[1];
            for ( God k = 1; k != candN; k++ )
            {
                if ( ggPos[2*k+1] > worstCandRs )
                {
                    worstCandRs = ggPos[2*k+1];
                    worstCandIdx = 2*k;
                }
            }
        }
    }


    // Do the j side.

    worstCandRs = 0;

    // These are guaranteed to be preliminary candidates.
    for ( God j = 0; j != candN; j++ )
    {
        uint64_t jRs = countRs( h, start + rs*n + posN*n + j, negN );

        jRs += countRs( h, start+j, rs );

        ggNeg[2*j] = j;
        ggNeg[2*j+1] = jRs;

        if ( jRs > worstCandRs )
        {
            worstCandRs = jRs;
            worstCandIdx = 2*j;
        }
    }

    for ( God j = candN; j != n; j++ )
    {
        uint64_t jRs = countRs( h, start + rs*n + posN*n + j, negN );

        jRs += countRs( h, start+j, rs );

        if ( jRs < worstCandRs )
        {
            ggNeg[worstCandIdx] = j;
            ggNeg[worstCandIdx+1] = jRs;

            // Find new worst.
            worstCandIdx = 0;
            worstCandRs = ggNeg[1];
            for ( God k = 1; k != candN; k++ )
            {
                if ( ggNeg[2*k+1] > worstCandRs )
                {
                    worstCandRs = ggNeg[2*k+1];
                    worstCandIdx = 2*k;
                }
            }
        }
    }


    God bestiGod;  // Best pos. god found so far.
    uint64_t bestiGodRs = UINT64_MAX;  // Number of Rs for bestiGod.
    uint64_t bestiGodRsrs;  // Number of Rs in rs for bestiGod.
    God bestjGod;  // Best neg. god found so far.
    uint64_t bestjGodRs = UINT64_MAX;  // Number of Rs for bestjGod.
    uint64_t bestjGodRsrs;  // Number of Rs in rs for bestjGod.

    // Loop through all combinations and see which pair is best.
    for ( GodsN k = 0; k != candN; k++ )
    {
        God i = ggPos[2*k];

        for ( GodsN m = 0; m != candN; m++ )
        {
            God j = ggNeg[2*m];

            // Try to swap conjunctions.
            if ( s->doSwaps > 0 )
            {
                swapConjunctionsG( h, i, start + rs * n,        posN,
                                      j, start + rs*n + posN*n, negN );

                if ( s->doSwaps > 1 )
                {
                    swapConjunctionsG3a( h, i, start + rs * n,        posN,
                                            j, start + rs*n + posN*n, negN );

                }

                if ( s->doSwaps > 2 )
                {
                    swapConjunctionsG3b( h, i, start + rs * n,        posN,
                                            j, start + rs*n + posN*n, negN );
                }                             

                if ( s->doSwaps > 3 )
                {
                    swapConjunctions0111( h, i, start + rs * n,        posN,
                                             j, start + rs*n + posN*n, negN );
                }                             

                if ( s->doSwaps > 4 )
                {
                    swapConjunctions1101( h, i, start + rs * n,        posN,
                                             j, start + rs*n + posN*n, negN );
                }                             

                // Re-count randoms in conclusions at gi for positive case, and 
                // gj for negative case, after the minimization.
                uint64_t iRs = countRs( h, start + rs * n + i,        posN );
                uint64_t jRs = countRs( h, start + rs*n + posN*n + j, negN );
                uint64_t iRsrs = countRs( h, start+i, rs );
                uint64_t jRsrs = countRs( h, start+j, rs );
                iRs += iRsrs;
                jRs += jRsrs;

                // See if pair is best.
                if ( iRs + jRs <= bestiGodRs + bestjGodRs )
                {
                    if ( iRs + jRs < bestiGodRs + bestjGodRs )
                    {
                        bestiGod = i;
                        bestiGodRs = iRs;
                        bestiGodRsrs = iRsrs;
                        bestjGod = j;
                        bestjGodRs = jRs;
                        bestjGodRsrs = jRsrs;
                    }
                    // Pick pairs where one side may get to 0 randoms.
                    else if ( s->doSwaps >= 8  &&  
                              ( iRsrs == 0 || jRsrs == 0 )  &&
                              bestiGodRsrs != 0  &&  bestjGodRsrs != 0 )
                    {
                        bestiGod = i;
                        bestiGodRs = iRs;
                        bestiGodRsrs = iRsrs;
                        bestjGod = j;
                        bestjGodRs = jRs;
                        bestjGodRsrs = jRsrs;
                    }
                }
            }
        }
    }


    *gi = bestiGod;
    *giRs = bestiGodRs;
    *gj = bestjGod;
    *gjRs = bestjGodRs;
}



// Counts randoms in conjuctions starting at start. 
//   rs is the number of special conjunctions at the top, where gq=R.
//   posN is the number of conjunctions in the positive half, not counting
// the rs block. negN is the number for the negative half. posN and negN might
// be 0.
//   Results for most promising gods will be placed in gi, giRs, gj, and gjRs.
// giRs and gjRs will include randoms in the rs block.
//   God gq is not skipped.
static void 
count2Rs( HardInstance * hi, uint64_t start, uint64_t rs,
                uint64_t posN, uint64_t negN,
                GodsN * gi, uint64_t * giRs, 
                GodsN * gj, uint64_t * gjRs /*, GodsN gq */ )
{
    Hard * h = hi->hard;
    //God * g = h->gods;
    GodsN n = h->godsN;
    Settings * s = hi->settings;


    if ( s->findGoodGods == 0 )
    {
        // Pick i and j at random. 

        uint64_t k = common_randomNBiased(n);
        *gi = k;

        k = common_randomNBiased(n-1);  // One could repeat until different from i case.
        *gj = k;

        // Count random gods for i and j.
        *giRs  = countRs( h, start + rs * n +            *gi, posN );
        *gjRs  = countRs( h, start + rs * n + posN * n + *gj, negN );
        *giRs += countRs( h, start + *gi, rs );
        *gjRs += countRs( h, start + *gj, rs );

        return;
    }


    if ( s->findGoodGods == 1  ||  s->goodGodsCandN <= 1 )
    {
        // Do the i side.

        God bestiGod;  // Best god found so far.
        //God sndBestiGod;  // Second best god so far. Not used atm.
        uint64_t bestiGodRs = UINT64_MAX;  // Number of Rs for bestiGod.
        //uint64_t sndBestiGodRs = UINT64_MAX;  // Number of Rs for sndBestiGod.

        for ( God i = 0; i != n; i++ )
        {
            uint64_t iRs = countRs( h, start + rs*n + i, posN );

            iRs += countRs( h, start+i, rs );

            //if ( iRs < sndBestiGodRs )
            {
                if ( iRs < bestiGodRs )
                {
                    //sndBestiGod = bestiGod;
                    //sndBestiGodRs = bestiGodRs;
                    bestiGod = i;
                    bestiGodRs = iRs;
                }
                else
                {
                    //sndBestiGod = i;
                    //sndBestiGodRs = iRs;
                }
            }
        }


        // Do the j side.

        God bestjGod;  // Best god found so far.
        //God sndBestjGod;  // Second best god so far. Not used atm.
        uint64_t bestjGodRs = UINT64_MAX;  // Number of Rs for bestjGod.
        //uint64_t sndBestjGodRs = UINT64_MAX;  // Number of Rs for sndBestjGod.

        for ( God j = 0; j != n; j++ )
        {
            uint64_t jRs = countRs( h, start + rs*n + posN*n + j, negN );

            jRs += countRs( h, start+j, rs );

            //if ( jRs < sndBestjGodRs )
            {
                if ( jRs < bestjGodRs )
                {
                    //sndBestjGod = bestjGod;
                    //sndBestjGodRs = bestjGodRs;
                    bestjGod = j;
                    bestjGodRs = jRs;
                }
                else
                {
                    //sndBestjGod = j;
                    //sndBestjGodRs = jRs;
                }
            }
        }

        *gi = bestiGod;
        *giRs = bestiGodRs;
        *gj = bestjGod;
        *gjRs = bestjGodRs;

        return;
    }


    if ( s->findGoodGods >= 2 )
    {
        count2RsQuad( hi, start, rs, posN, negN, gi, giRs, gj, gjRs );

        return;
    }

}



// Moves disjuncts with gq=R to the top/start.  
//   start is the start of a disjunction, end is the 
// end (pointing to the cell after the disjunction). 
// rs is the number of randoms at conjunct gq. rs should be > 0.
static void rSort( Hard * h, uint64_t start, uint64_t end, 
                   GodsN gq, uint64_t rs )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t fstNonR = start;  // The first conjunction with non-R at gq.


    // There are non-random gods at gq. Or should be. But maybe with
    // poor choices there might not be? With random choices e.g., -g 0.
    //assert( ( end - start ) / n > rs );
    // But if there are non, return.
    if ( ( end - start ) / n  ==  rs )
    {
        return;
    } 


    // Find first non-random. Also count down Rs already at the top.
    for ( ; godV( g, fstNonR + gq ) == randomGod; fstNonR += n )
    {
        rs--;
    }

    assert( fstNonR < end );

    if Unlikely( rs == 0 )
    {
        // We are already done.

        return;
    }


    uint64_t lastR = end-n;  // The last conjunction with R at gq.

    // Find last random.
    for ( ; godV( g, lastR + gq ) != randomGod; lastR -= n )
    {
    }

    assert( fstNonR < lastR );


    // Swap conjuntions.
    swapConjs( h, fstNonR, lastR );
    rs--;
    fstNonR += n;
    lastR -= n;


    // Repeatedly swap until Rs are all at the top.
    //   The algorithm should have just gone on until fstNonR and lastR 
    // pass each other instead, maybe, instead of messing with rs. Also,
    // the initialization and first swap is nothing special and could
    // have been handled in the loop.
    while ( rs != 0 )
    {
        // Find first non-random. Also count down Rs already at the top.
        for ( ; godV( g, fstNonR + gq ) == randomGod; fstNonR += n )
        {
            rs--;

            if ( rs == 0 )
            {
                return;
            }
        }

        // Find last random.
        for ( ; godV( g, lastR + gq ) != randomGod; lastR -= n )
        {
        }

        assert( fstNonR < lastR );

        // Swap conjunctions.
        swapConjs( h, fstNonR, lastR );
        rs--;
        fstNonR += n;
        lastR -= n;
    }
}                   



// Moves disjuncts with gq=R to the bottom/end.  
//   start is the start of a disjunction, end is the 
// end (pointing to the cell after the disjunction). 
// end is meant to be the start of the neg block.
// rs is the number of randoms at conjunct gq.
static void rSortb( Hard * h, uint64_t start, uint64_t end, 
                    GodsN gq, uint64_t rs )
{
    GodsN n = h->godsN;
    God * g = h->gods;

    uint64_t lastNonR = end-n;  // The last conjunction with non-R at gq.
    uint64_t fstR = start;  // The first conjunction with R at gq.

    // There are non-random gods at gq. Or should be.
    //assert( ( end - start ) / n > rs );
    // But if there are non, return.
    if ( ( end - start ) / n  ==  rs )
    {
        return;
    } 

    // Repeatedly swap until Rs are all last.
    //   The algorithm should have just gone on until lastNonR and fstR 
    // pass each other instead, maybe, instead of messing with rs. This
    // is fine though.
    while ( rs != 0 )
    {
        // Find last non-random. Also count down Rs already last.
        for ( ; godV( g, lastNonR + gq ) == randomGod; lastNonR -= n )
        {
            rs--;

            if ( rs == 0 )
            {
                return;
            }
        }

        // Find first random.
        for ( ; godV( g, fstR + gq ) != randomGod; fstR += n )
        {
        }

        assert( fstR < lastNonR );

        // Swap conjunctions.
        swapConjs( h, fstR, lastNonR );
        rs--;
        lastNonR -= n;
        fstR += n;
    }
}                   



// Shuffles conjunctions starting at start, and ending at end.
//   Uses the Fisher-Yates shuffle.
static void shuffleConjunctions( uint64_t start, uint64_t end, Hard * h )
{
    GodsN n = h->godsN;
    uint64_t size = ( end - start ) / n;

    if ( size == 0 )
    {
        return;
    }

    for ( uint64_t m = size-1; m != 0; m-- )
    {
        uint64_t j = common_randomN(m+1);

        if ( j != m )
        {
            swapConjs( h, start + j*n, start + m*n );
        }
    }
}



// Finds one solution to the generalization of "Hardest Logic Puzzle Ever". 
//   The algorithm will be derived from the paper
// 'How to Solve "The Hardest Logic Puzzle Ever" and Its
// Generalization'.
//   Like find1 but with no initialization, but with recursive calls at the
// end. See find1 first.
//   start is the start of the disjunction that holds, end is the 
// end (pointing to the cell after the disjunction). gq is
// the god to be questioned. rs is the number of randoms at conjunct gq.
//   qs is the number of questions posed so far, not counting the upcoming
// question to gq.
//   (E.g. start and end are indexes into h->gods, starting at 0, with end
// pointing to cell after batch.)
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_resultFound
// otherwise.
static uint8_t fnd1( HardInstance * hi, uint64_t start, uint64_t end, 
                     GodsN gq, uint64_t rs, GodsN qs )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    GodsN n = h->godsN;
    //GodsN f = h->fGodsN;
    //GodsN t = h->tGodsN;
    //GodsN r = h->rGodsN;

    God * g = h->gods;


    assert3( rs  ==  countRs( h, start + gq, ( end - start ) / n ) );

    assert( end > start );

    
    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        fputc( '\n', s->outFile );
        indent( s->indent * qs, s->outFile );
        fprintf( s->outFile, "asking g%u, with %lu disjuncts left:\n", 
                 gq, (end-start) / n );
    }


    if ( rs == 0 )
    {
        // Answers don't have to be weakened with gq=R. We'll do binary
        // splitting of the remaining disjuncts.

        // Print info.
        if ( s->verbosityVector & HardVerbosity_printAll )
        {
            indent( s->indent * qs, s->outFile );
            fprintf( s->outFile, 
                     "g%u is non-random, with log2(%llu)=%3.*f.\n",
                     gq, (unsigned long long)( (end-start) / n ),
                     s->precision, log2( (end-start) / n ) );
        }


        // Add that we are questioning gq. Not needed here though.
        //h->qPath[qs] = gq;


        // We'll try to get conjunctions that can be reached in multiple
        // ways to have longer paths. We'll put those last. 

        // Calculate how many of the conjunctions can get the floor log,
        // and how many get the ceiling log.
        uint64_t conjs = (end-start) / n;  // Number of conjunctions.
        uint64_t logconjs = log2(conjs);
        uint64_t conjsCeiling = ((uint64_t)1) << (logconjs+1);

        // How many that get the floor log discovery.
        uint64_t floorLogs = conjsCeiling - conjs; 

        uint8_t qsn = qs + logconjs;  // Current number of questions.

        // New start of unhandled conjunctions. Handled conjunctions will
        // be swapped to top, except at the end.
        uint64_t strt = start;

        // This case should be handled below.
        #if 0
        if ( s->optimizeNonR == 0 )  // This case is handled below.
        {
            uint64_t i = start;  // The current conjunction.

            // Just assign floor logs to the first disjuncts.
            for ( ; floorLogs != 0; floorLogs--, i += n )
            {
                if ( conjHash_add( i, qsn, h ) )
                {
                    fprintf( stderr, "\nError: not enough memory.\n\n" );

                    return true;
                }
            }

            // Do the rest of the disjuncts.
            for ( qsn++; i != end; i += n )
            {
                if ( conjHash_add( i, qsn, h ) )
                {
                    fprintf( stderr, "\nError: not enough memory.\n\n" );

                    return true;
                }
            }

            return false;
        }
        #endif

        // Give the floor log discoveries to the most important conjunctions,
        // and the ceiling discoveries to the rest.
        //   Disjuncts with many pathways won't get detected right away.
        // Ideally one should defer and commit when more is known. However,
        // solving just this relatively simple optimization might be hard,
        // in a complexity sense? And getting this optimal shouldn't matter
        // that much?
        //   In fact, now, with some bookkeeping, we can calculate the 
        // exact probability or importance of a detection, and we'll use
        // that to order disjuncts according to importance.

        for ( uint8_t o = 0; o != s->optimizeNonR; o++ )
        {
            uint64_t i;  // The current conjunction.

            // First, give out floor logs to conjunctions with 
            // 1/2^o probabilty to get detected this way.
            for ( i = strt; floorLogs != 0  &&  i != end; i += n )
            {
                uint8_t result = conjHash_addIf( i, qsn, hi, qs, o );

                if ( result == Hard_outOfMemory )
                {
                    fprintf( stderr, "\nError: not enough memory.\n\n" );

                    return Hard_outOfMemory;
                }

                if ( result == Hard_aborted )
                {
                    return Hard_aborted;
                }

                if ( result == Hard_pathAdded )
                {
                    // Swap conjunctions.
                    if ( i != strt )
                    {
                        swapConjs( h, strt, i );
                    }

                    strt += n;
                    floorLogs--;
                }
            }

            if ( floorLogs == 0 )
            {
                // Do the rest of the disjuncts.
                for ( qsn++, i = strt; i != end; i += n )
                {
                    uint8_t result = conjHash_add( i, qsn, hi, qs );

                    if ( result == Hard_outOfMemory )
                    {
                        fprintf( stderr, "\nError: not enough memory.\n\n" );

                        return Hard_outOfMemory;
                    }

                    if ( result == Hard_aborted )
                    {
                        return Hard_aborted;
                    }
                }

                return Hard_resultFound;
            }
        }

        // Give up and just assign floor logs to the first conjunctions,
        // and ceilings to the rest. This should not happen, unless you
        // have a very low o.

        uint64_t i = strt;  // The current conjunction.

        for ( ; floorLogs != 0; floorLogs--, i += n )
        {
            uint8_t result = conjHash_add( i, qsn, hi, qs );

            if ( result == Hard_outOfMemory )
            {
                fprintf( stderr, "\nError: not enough memory.\n\n" );

                return Hard_outOfMemory;
            }

            if ( result == Hard_aborted )
            {
                return Hard_aborted;
            }
        }

        // Do the rest of the disjuncts.
        for ( qsn++; i != end; i += n )
        {
            uint8_t result = conjHash_add( i, qsn, hi, qs );

            if ( result == Hard_outOfMemory )
            {
                fprintf( stderr, "\nError: not enough memory.\n\n" );

                return Hard_outOfMemory;
            }

            if ( result == Hard_aborted )
            {
                return Hard_aborted;
            }
        }

        return Hard_resultFound;
    }


    if Unlikely( (end-start) / n  ==  1 )
    {
        // This case should never happen? Maybe it can, with poor
        // choices.
        
        // Print info.
        if ( s->verbosityVector & HardVerbosity_printAll )
        {
            indent( s->indent * qs, s->outFile );
            fprintf( s->outFile, 
                     "There is only one disjunct left.\n" );
        }

        return conjHash_add( start, qs, hi, qs );
    }


    // We'll resort disjuncts, with gq=R all at the top,  
    // in the current disjunction.
    //   After the positive case we'll reshift the gq=R 
    // disjuncts to the top of the neg block.

    rSort( h, start, end, gq, rs );

    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        indent( s->indent * qs, s->outFile );
        fprintf( s->outFile, "g%u=R possibilities: %llu\n", 
                 gq, (unsigned long long)rs );

        indent( s->indent * qs, s->outFile );
        fprintf( s->outFile, "local premise <->\n" );

        indent( s->indent * qs, s->outFile );
        for ( uint64_t i = start; i != end; )
        {
            printGod( godVal( g[godSlot(i)], godCell(i) ), s->outFile );

            i++;

            if ( i % n == 0 )
            {
                fputc( '\n', s->outFile );
                indent( s->indent * qs, s->outFile );
            }
        }

        fputc( '\n', s->outFile );
    }


    // Maybe shuffle conjunctions.

    if ( s->shuffleConjunctions == 4  ||  s->shuffleConjunctions > 5 )
    {
        shuffleConjunctions( start + rs*n, end, h );
    }

    if ( s->shuffleConjunctions >= 5 )
    {
        shuffleConjunctions( start, start + rs*n, h );
    }


    // If the number of conjunctions is odd, we'll have to decide how to 
    // split them.

    // Number of conjunctions in the current disjunction premise, with
    // rs conjunctions counted twice.
    uint64_t conjsN = ( end - start ) / n  +  rs;  // rs is counted twice.
    assert( ( end - start ) % n  ==  0 );

    uint64_t midConj = conjsN/2;

    bool fstHasOneMoreConj = false;
    bool sndHasOneMoreConj = false;

    if ( conjsN % 2 != 0 )
    {
        if ( s->oddBias == 1  ||  s->oddBias == 0 &&
             common_randomNBiasedNonPeriodic(2) )
        {
            fstHasOneMoreConj = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                indent( s->indent * qs, s->outFile );
                fprintf( s->outFile, 
                         "odd #: positive case has one more conjunction\n" );
            }
        }
        else
        {
            sndHasOneMoreConj = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                indent( s->indent * qs, s->outFile );
                fprintf( s->outFile, 
                         "odd #: negative case has one more conjunction\n" );
            }
        }
    }


    // The start of the conclusion of a negative answer to the upper question,
    // including the possiblity that gq=R. Right now; for the positive case.
    // For the negative case, rs conjunctions will be moved down, to 
    // the top of the negative block.
    uint64_t midStart = start + ( midConj + fstHasOneMoreConj ) * n;

    // The start of the negative case, after rs conjunctions have been
    // moved down.
    uint64_t negStart = midStart - rs*n; 
    
    // Base number of conjunctions in a question, not including gq=R 
    // possibilities, and not counting fstHas or sndHas.
    uint64_t qConjs = conjsN / 2 - rs;


    // Next we'll try to find gPos (gi) and gNeg (gj) with fewest randoms.
    // We'll count all disjuncts, also top randoms (gq), but we'll 
    // swap only "middles".
    //   The non-rs parts of the positive and negative cases might be
    // empty. (But rs is not 0.)


    // Count randoms in conclusions at gi for positive case, and gj for
    // negative case. We'll try to minimize those, later.
    //   Random starting conjunctions will not be ignored.

    GodsN gi;  // The god we'll go after next in the positive case.
    GodsN gj;  // The god we'll go after next in the negative case.
    uint64_t giRs;  // Number of Rs at i (gi) in the positive case.
    uint64_t gjRs;  // Number of Rs at j (gj) in the negative case.

    count2Rs( hi, start, rs, qConjs + fstHasOneMoreConj,
                             qConjs + sndHasOneMoreConj,
              &gi, &giRs, &gj, &gjRs );

    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        /*
        if ( fstHasOneMoreConj )
        { 
            fprintf( s->outFile, 
                     "split so that q has one more conjunction\n" );
       
        }

        if ( sndHasOneMoreConj )
        { 
            fprintf( s->outFile, 
                     "split so that -q has one more conjunction\n" );
       
        }
        */

        indent( s->indent * qs, s->outFile );
        fprintf( s->outFile, "randoms at g%u for  q (counting g%u=R): %lu\n",
                             gi, gq, giRs );
        indent( s->indent * qs, s->outFile );
        fprintf( s->outFile, "randoms at g%u for -q (counting g%u=R): %lu\n",
                             gj, gq, gjRs );
    }


    // Try to swap conjunctions.
    if ( s->doSwaps > 0 )
    {
        swapConjunctionsG( h, gi, start + rs * n, qConjs + fstHasOneMoreConj,
                              gj, midStart,       qConjs + sndHasOneMoreConj );

        if ( s->doSwaps > 1 )
        {
            swapConjunctionsG3a( h, gi, start + rs * n, qConjs + fstHasOneMoreConj,
                                    gj, midStart,       qConjs + sndHasOneMoreConj );

        }

        if ( s->doSwaps > 2 )
        {
            swapConjunctionsG3b( h, gi, start + rs * n, qConjs + fstHasOneMoreConj,
                                    gj, midStart,       qConjs + sndHasOneMoreConj );
        }                             

        if ( s->doSwaps > 3 )
        {
            swapConjunctions0111( h, gi, start + rs * n, qConjs + fstHasOneMoreConj,
                                     gj, midStart,       qConjs + sndHasOneMoreConj );
        }                             

        if ( s->doSwaps > 4 )
        {
            swapConjunctions1101( h, gi, start + rs * n, qConjs + fstHasOneMoreConj,
                                     gj, midStart,       qConjs + sndHasOneMoreConj );
        }                             

        
        // Re-count randoms in conclusions at gi for positive case, and gj for
        // negative case, after the minimization.
        //   Random starting conjunctions will not be ignored.
        giRs = countRs( h, start + rs * n + gi, 
                           qConjs + fstHasOneMoreConj );
        gjRs = countRs( h, midStart + gj, 
                           qConjs + sndHasOneMoreConj );
        uint64_t giRsNonrs = giRs;  // For trying to get Rs to 0.
        uint64_t gjRsNonrs = gjRs;  // For trying to get Rs to 0.
        giRs += countRs( h, start+gi, rs );
        gjRs += countRs( h, start+gj, rs );


        // Print info.
        if ( hi->settings->verbosityVector & HardVerbosity_printAll )
        {
            fputc( '\n', s->outFile );
            indent( s->indent * qs, s->outFile );
            fprintf( s->outFile, "after swapping (not printing g%u=R "
                                 "conjunctions):\n", gq );

            // For knowing when to print ---.
            // iMid is the conjunction where neg half begins.
            uint64_t iMid = ( ( end - ( start + n*rs ) ) / n / 2  +
                              fstHasOneMoreConj )  *  n  +  
                            ( start + n * rs );
            if ( iMid == start + n * rs )
            {
                indent( s->indent * qs, s->outFile );
                for ( uint8_t j = 0; j != n; j++ )
                {
                    fputc( '-', s->outFile );
                }

                fputc( '\n', s->outFile );
            }                                         

            indent( s->indent * qs, s->outFile );
            for ( uint64_t i = start + n * rs; i != end; )
            {
                printGod( godVal( g[godSlot(i)], godCell(i) ), s->outFile );

                i++;

                if ( i % n == 0 )
                {
                    fputc( '\n', s->outFile );

                    if ( i == iMid )
                    {
                        indent( s->indent * qs, s->outFile );
                        for ( uint8_t j = 0; j != n; j++ )
                        {
                            fputc( '-', s->outFile );
                        }

                        fputc( '\n', s->outFile );
                    }

                    indent( s->indent * qs, s->outFile );
                }
            }

            fputc( '\n', s->outFile );

            indent( s->indent * qs, s->outFile );
            fprintf( s->outFile, "randoms at g%u for  q (counting g%u=R): %lu\n",
                                 gi, gq, giRs );
            indent( s->indent * qs, s->outFile );
            fprintf( s->outFile, "randoms at g%u for -q (counting g%u=R): %lu\n",
                                 gj, gq, gjRs );
        }


        // Try to get a side to 0 randoms. We'll only try if there are
        // no randoms in rs.
        if ( s->doSwaps >= 8  &&  giRs + gjRs <= s->maxUnbal  &&
             giRs != 0  &&  gjRs != 0  &&  
             ( giRs == giRsNonrs  ||  gjRs == gjRsNonrs ) )
        {
            if ( giRs <= gjRs  &&  giRs == giRsNonrs  ||  gjRs != gjRsNonrs )
            {
                swapConjunctionsG2a( h,   gi,   start + rs * n, qConjs + fstHasOneMoreConj,
                                        /*gj,*/ midStart,       qConjs + sndHasOneMoreConj,
                                     giRs );
                
            }
            else
            {
                swapConjunctionsG2b( h, /*gi,*/ start + rs * n, qConjs + fstHasOneMoreConj,
                                          gj,   midStart,       qConjs + sndHasOneMoreConj,
                                     gjRs );
            }

            // Re-count randoms in conclusions at gi for positive case, and gj for
            // negative case, after the minimization.
            //   Random starting conjunctions will not be ignored.
            giRs = countRs( h, start + rs * n + gi, 
                            qConjs + fstHasOneMoreConj );
            gjRs = countRs( h, midStart + gj, 
                            qConjs + sndHasOneMoreConj );
            //giRsNonrs = giRs;  // For trying to get Rs to 0.
            //gjRsNonrs = gjRs;  // For trying to get Rs to 0.
            giRs += countRs( h, start+gi, rs );
            gjRs += countRs( h, start+gj, rs );

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                indent( s->indent * qs, s->outFile );
                fprintf( s->outFile,
                            "unbalancing... randoms at g%u for  q (counting g%u=R): %lu\n",
                            gi, gq, giRs );
                indent( s->indent * qs, s->outFile );
                fprintf( s->outFile,
                            "unbalancing... randoms at g%u for -q (counting g%u=R): %lu\n",
                            gj, gq, gjRs );
            }
        }


        // Do "non-improving" swaps, to balance sides.
        //   (If all randoms are in rs, there is no point trying to swap.
        // This is not checked. It probably doesn't matter.)
        if ( s->doSwaps == 7  ||  ( s->doSwaps == 6 || s->doSwaps >= 8 ) &&
                                  giRs != 0 && gjRs != 0 )
        {
            if ( giRs + 1 < gjRs )
            {
                swapConjunctionsG2b( h, /*gi,*/ start + rs * n, qConjs + fstHasOneMoreConj,
                                          gj,   midStart,       qConjs + sndHasOneMoreConj,
                                        (gjRs-giRs) / 2 );
            }
            else if ( gjRs + 1 < giRs )
            {
                swapConjunctionsG2a( h,   gi,   start + rs * n, qConjs + fstHasOneMoreConj,
                                        /*gj,*/ midStart,       qConjs + sndHasOneMoreConj,
                                        (giRs-gjRs) / 2 );
            }

            if ( giRs + 1 < gjRs  ||  gjRs + 1 < giRs )
            {
                // Re-count randoms in conclusions at gi for positive case, 
                // and gj for negative case, after the minimization.
                //   Random starting conjunctions will not be ignored.
                giRs = countRs( h, start + rs * n + gi, 
                                qConjs + fstHasOneMoreConj );
                gjRs = countRs( h, midStart + gj, 
                                qConjs + sndHasOneMoreConj );
                giRs += countRs( h, start+gi, rs );  // Unchanged. Maybe save
                gjRs += countRs( h, start+gj, rs );  // values instead.


                // Print info.
                if ( hi->settings->verbosityVector & HardVerbosity_printAll )
                {
                    fputc( '\n', s->outFile );
                    indent( s->indent * qs, s->outFile );
                    fprintf( s->outFile, "after balancing (not printing "
                                         "g%u=R conjunctions):\n", gq );

                    // For knowing when to print ---.
                    // iMid is the conjunction where neg half begins.
                    uint64_t iMid = ( ( end - ( start + n*rs ) ) / n / 2  +
                                      fstHasOneMoreConj )  *  n  +
                                    ( start + n * rs );
                    if ( iMid == start + n * rs )
                    {
                        indent( s->indent * qs, s->outFile );
                        for ( uint8_t j = 0; j != n; j++ )
                        {
                            fputc( '-', s->outFile );
                        }

                        fputc( '\n', s->outFile );
                    }                                         

                    indent( s->indent * qs, s->outFile );
                    for ( uint64_t i = start + n * rs; i != end; )
                    {
                        printGod( godVal( g[godSlot(i)], godCell(i) ),
                                  s->outFile );

                        i++;

                        if ( i % n == 0 )
                        {
                            fputc( '\n', s->outFile );

                            if ( i == iMid )
                            {
                                indent( s->indent * qs, s->outFile );
                                for ( uint8_t j = 0; j != n; j++ )
                                {
                                    fputc( '-', s->outFile );
                                }

                                fputc( '\n', s->outFile );
                            }

                            indent( s->indent * qs, s->outFile );
                        }
                    }

                    fputc( '\n', s->outFile );

                    indent( s->indent * qs, s->outFile );
                    fprintf( s->outFile,
                             "randoms at g%u for  q (counting g%u=R): %lu\n",
                             gi, gq, giRs );
                    indent( s->indent * qs, s->outFile );
                    fprintf( s->outFile,
                             "randoms at g%u for -q (counting g%u=R): %lu\n",
                             gj, gq, gjRs );
                }
            }
        }                             
    }


    // Add that we are questioning gq.
    h->qPath[qs] = gq;
    

    // State before positive call.
    uint64_t baseSubResN;
    double baseSubResSum;
    double baseAbortLeeway;

    // Counts how many active marks there are (this call). 
    uint8_t markCounter = 0;

    // Mark and save state, if there are randoms.
    if ( giRs != 0  &&  s->lvlReps[qs] != 0 )
    {
        // We'll do either a local or a global mark.
        if ( qs >= s->topLocalResetLevel )
        {
            // Mark only locally.
            if ( conjHash_markStateLocally( h, start, end ) )
            { 
                return Hard_outOfMemory;
            }
        }
        else
        {
            if ( conjHash_markState(h) )
            { 
                return Hard_outOfMemory;
            }
        }


        markCounter++;

        baseSubResN = h->subresultsFound;
        baseSubResSum = h->subresSum;

        //uint64_t posCatchAbortsN = h->catchAbortsN;
        baseAbortLeeway = h->abortLeeway;    
        //posAbortLeewayDecrement = h->abortLeewayDecrement;
    }

    // Handle the positive case.
    uint8_t result = fnd1( hi, start, midStart, gi, giRs, qs+1 );

    if ( result == Hard_outOfMemory )
    {
        return Hard_outOfMemory;
    }

    if ( result == Hard_aborted )
    {
        if ( markCounter != 0 )
        {
            assert( markCounter == 1 );

            // State is marked either locally or globally.
            if ( qs >= s->topLocalResetLevel )
            {
                // Undo only locally.
                conjHash_removeLastLocalMark( h, start, end );
            }
            else
            {
                conjHash_removeLastMark(h);
            }
        }

        return Hard_aborted;
    }

    // True iff state is marked, i.e. a mark precedes all paths.
    bool stateIsMarked = false;


    // Repeat and keep best try.
    if ( giRs != 0  &&  s->lvlReps[qs] != 0 )
    {
        // Save best state.
        uint64_t bestSubResN = h->subresultsFound;
        double bestSubResSum = h->subresSum;
        double bestAbortLeeway = h->abortLeeway;

        // State is marked either locally or globally.
        if ( qs >= s->topLocalResetLevel )
        {
            // Mark only locally.
            if ( conjHash_markStateLocally( h, start, end ) )
            { 
                return Hard_outOfMemory;
            }
        }
        else
        {
            if ( conjHash_markState(h) )
            { 
                return Hard_outOfMemory;
            }
        }

        markCounter++;

        for ( uint16_t i = 0; i != s->lvlReps[qs]; i++)
        {
            // Reset state.
            h->subresultsFound = baseSubResN;
            h->subresSum = baseSubResSum;
            h->abortLeeway = baseAbortLeeway;

            // Do we want to shuffle??

            result = fnd1( hi, start, midStart, gi, giRs, qs+1 );

            if ( result == Hard_outOfMemory )
            {
                return Hard_outOfMemory;
            }

            if ( result != Hard_aborted  &&
                 (double)h->subresSum / h->subresultsFound  <
                 (double)bestSubResSum / bestSubResN )
            {
                // State is marked either locally or globally.
                if ( qs >= s->topLocalResetLevel )
                {
                    // Undo only locally.
                    conjHash_undo2Locally( h, start, end );

                    // should be done in undo2Locally!!vvv
                    if ( conjHash_markStateLocally( h, start, end) )
                    { 
                        return Hard_outOfMemory;
                    }
                }
                else
                {                    
                    conjHash_undo2(h);

                    if ( conjHash_markState(h) )  // should be done in undo2!!vvv
                    { 
                        return Hard_outOfMemory;
                    }
                }

                bestSubResN = h->subresultsFound;
                bestSubResSum = h->subresSum;
                bestAbortLeeway = h->abortLeeway;
            }
            else
            {
                // State is marked either locally or globally.
                if ( qs >= s->topLocalResetLevel )
                {
                    // Undo only locally.
                    conjHash_undoLocally( h, start, end );
                }
                else
                {
                    conjHash_undo(h);
                }
            }
        }        

        // Update state.
        h->subresultsFound = bestSubResN;
        h->subresSum = bestSubResSum;
        h->abortLeeway = bestAbortLeeway;

        //conjHash_removeLastMark(h);
        stateIsMarked = true;
    }


    // Update best estimates.
    // vvv implement call sequence!!! only lvl 0 works now. see hard.h.
    #if 0
    if ( (double)(h->subresSum) / h->subresultsFound  < 
         h->bestPositiveEstimates[0] )
    {
        h->bestPositiveEstimates[0] = (double)(h->subresSum) / 
                                      h->subresultsFound;
    } 
    #endif

    // Save state.
    //uint64_t posCatchAbortsN = h->catchAbortsN;
    baseAbortLeeway = h->abortLeeway;    
    //posAbortLeewayDecrement = h->abortLeewayDecrement;
    baseSubResN = h->subresultsFound;
    baseSubResSum = h->subresSum;

    // vvv implement call sequence!!! only lvl 0 works now. see hard.h.
    #if 0
    // Check if state is promising.

    bool stateIsPromising = false;

    //double posAbortLeeway;    
    //double posAbortLeewayDecrement;
    //uint64_t posSubresultsFound;
    //uint64_t posSubresSum;

    if ( (double)(h->subresSum) / h->subresultsFound  <
         h->bestPositiveEstimates[0] * s->resumeAbortedLeeway )
    {
        stateIsPromising = true;

        // Print info.
        if ( hi->settings->verbosityVector & HardVerbosity_printAll )
        {
            fprintf( s->outFile, "state is promising: estimated result: %f\n",
                     (double)(h->subresSum) / h->subresultsFound );
        }
    }
    #endif

    if ( !stateIsMarked  &&  gjRs != 0  &&  s->lvlReps[qs] != 0 )
    {
        // State is marked either locally or globally.
        if ( qs >= s->topLocalResetLevel )
        {
            // Mark only locally.
            if ( conjHash_markStateLocally( h, start, end ) )
            { 
                return Hard_outOfMemory;
            }
        }
        else
        {
            if ( conjHash_markState( h ) )
            { 
                return Hard_outOfMemory;
            }
        }

        stateIsMarked = true;
        markCounter++;
    }


    // Move the rs block to the top of the neg block.
    rSortb( h, start, midStart, gq, rs );


    // Handle the negative case.

    result = fnd1( hi, negStart, end, gj, gjRs, qs+1 );

    stateIsMarked = false;

    if ( result == Hard_outOfMemory )
    {
        return Hard_outOfMemory;
    }

    if ( result == Hard_aborted )
    {
        // Remove recent marks.
        for ( ; markCounter != 0; markCounter-- )
        {
            // State is marked either locally or globally.
            if ( qs >= s->topLocalResetLevel )
            {
                // Undo only locally.
                conjHash_removeLastLocalMark( h, start, end );
            }
            else
            {
                conjHash_removeLastMark(h);
            }
        }

        return Hard_aborted;
    }


    // Repeat and keep best try.
    if ( gjRs != 0  &&  s->lvlReps[qs] != 0 )
    {
        // Save best state.
        uint64_t bestSubResN = h->subresultsFound;
        double bestSubResSum = h->subresSum;
        double bestAbortLeeway = h->abortLeeway;

        if ( !stateIsMarked )
        {
            // State is marked either locally or globally.
            if ( qs >= s->topLocalResetLevel )
            {
                // Mark only locally.
                if ( conjHash_markStateLocally( h, start, end ) )
                { 
                    return Hard_outOfMemory;
                }
            }
            else
            {
                if ( conjHash_markState(h) )
                { 
                    return Hard_outOfMemory;
                }
            }

            markCounter++;
        }

        for ( uint16_t i = 0; i != s->lvlReps[qs]; i++)
        {
            // Reset state.
            h->subresultsFound = baseSubResN;
            h->subresSum = baseSubResSum;
            h->abortLeeway = baseAbortLeeway;

            // Do we want to shuffle??

            result = fnd1( hi, negStart, end, gj, gjRs, qs+1 );

            if ( result == Hard_outOfMemory )
            {
                return Hard_outOfMemory;
            }

            if ( result != Hard_aborted  &&
                 (double)h->subresSum / h->subresultsFound  <
                 (double)bestSubResSum / bestSubResN )
            {
                // State is marked either locally or globally.
                if ( qs >= s->topLocalResetLevel )
                {
                    // Undo only locally.
                    conjHash_undo2Locally( h, start, end );

                    // should be done in undo2Locally!!vvv
                    if ( conjHash_markStateLocally( h, start, end) )
                    { 
                        return Hard_outOfMemory;
                    }
                }
                else
                {                    
                    conjHash_undo2(h);

                    if ( conjHash_markState(h) )  // should be done in undo2!!vvv
                    { 
                        return Hard_outOfMemory;
                    }
                }
                
                bestSubResN = h->subresultsFound;
                bestSubResSum = h->subresSum;
                bestAbortLeeway = h->abortLeeway;
            }
            else
            {
                // State is marked either locally or globally.
                if ( qs >= s->topLocalResetLevel )
                {
                    // Undo only locally.
                    conjHash_undoLocally( h, start, end );
                }
                else
                {
                    conjHash_undo(h);
                }
            }
        }        

        // Update state.
        h->subresultsFound = bestSubResN;
        h->subresSum = bestSubResSum;
        h->abortLeeway = bestAbortLeeway;

        //conjHash_removeLastMark(h);
        stateIsMarked = true;
    }

    // Remove recent marks.
    for ( ; markCounter != 0; markCounter-- )
    {
        // State is marked either locally or globally.
        if ( qs >= s->topLocalResetLevel )
        {
            // Undo only locally.
            conjHash_removeLastLocalMark( h, start, end );
        }
        else
        {
            conjHash_removeLastMark(h);
        }
    }


    return result;
}



// Finds one solution to the generalization of "Hardest Logic Puzzle Ever". 
//   The algorithm will be derived from the paper
// 'How to Solve "The Hardest Logic Puzzle Ever" and Its
// Generalization'.
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_resultFound
// otherwise.
static uint8_t find1( HardInstance * hi )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    GodsN n = h->godsN;
    GodsN f = h->fGodsN;
    GodsN t = h->tGodsN;
    GodsN r = h->rGodsN;

    //uint64_t cellsUsed0 = h->cellsUsed;
    //uint64_t slotsUsed0 = h->slotsUsed;


    if Unlikely( n == 1 )
    {
        if ( s->verbosityVector & HardVerbosity_printResult )
        {
            fprintf( s->outFile, "There is only one god.\n" );
        }

        return Hard_aborted;  // ??
    }


    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        fprintf( s->outFile, "\nasking g0:\n" );
    }


    // Create g0=R. (Where gi is the ith god.)

    // Number of possibilities with g0=R.
    uint64_t poss0R0 = possSafe( n-1, f, t );

    // Print info.
    if ( s->verbosityVector & HardVerbosity_printAll )
    {
        fprintf( s->outFile, "g0=R possibilities: %llu\n", 
                 (unsigned long long)poss0R0 );
    }

    // Check god memory bound.
    if Unlikely( checkMemoryBound( hi, poss0R0 * n ) )
    {
        return Hard_outOfMemory;
    }

    God * g = h->gods;

    setGodIncH( h, randomGod );
    h->godsInConj++;

    addToPrefix( h, randomGod );

    setPoss( f, t, r-1, h );

    h->godsInPrefix--;
    assert( h->godsInPrefix == 0 );


    uint64_t totalCellsUsed = cellsUsedTot(h);

    assert( totalCellsUsed == n * poss0R0 );


    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        fprintf( s->outFile, "g0=R <->\n" );

        for ( uint64_t i = 0; i != totalCellsUsed; )
        {
            printGod( godVal( g[godSlot(i)], godCell(i) ), s->outFile );

            i++;

            if ( i % n == 0 )
            {
                fputc( '\n', s->outFile );
            }
        }

        fputc( '\n', s->outFile );
    }


    // Create all possibilities.

    //uint64_t poss0 = possSafe( n, f, t );

    // Check god memory bound.
    if Unlikely( checkMemoryBound( hi, h->possN * n ) )
    {
        return Hard_outOfMemory;
    }

    g = h->gods;

    setPoss( f, t, r, h );

    assert( h->godsInPrefix == 0 );

    totalCellsUsed = cellsUsedTot(h);

    assert( totalCellsUsed == n * poss0R0  +  n * h->possN );


    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        fprintf( s->outFile, "all possibilities <->\n" );

        for ( uint64_t i = n * poss0R0; i != totalCellsUsed; )
        {
            printGod( godVal( g[godSlot(i)], godCell(i) ), s->outFile );

            i++;

            if ( i % n == 0 )
            {
                fputc( '\n', s->outFile );
            }
        }

        fputc( '\n', s->outFile );
    }


    // Initialize the hash table.
    if ( conjHash_initHash( h, poss0R0 ) )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );
        
        return Hard_outOfMemory;
    }

    // Print info.
    if ( s->verbosityVector & HardVerbosity_printAll )
    {
        fprintf( s->outFile, "log size of hash table: %u\n", 
                 (unsigned)h->ht->logSize );

        fprintf( s->outFile, "number of hash table collisions: %llu\n", 
                 (unsigned long long)h->ht->nrOfCollisions );

        conjHash_stats( h, s->outFile );                 
    }


    // Maybe shuffle conjunctions.

    if ( s->shuffleConjunctions == 1  ||  s->shuffleConjunctions > 2 )
    {
        shuffleConjunctions( poss0R0 * n, h->possN * n, h );
    }

    if ( s->shuffleConjunctions >= 2 )
    {
        shuffleConjunctions( 0, poss0R0 * n, h );
        shuffleConjunctions( h->possN * n, totalCellsUsed, h );
    }



    // If the number of conjunctions is odd, we'll have to decide how to 
    // split them.

    uint64_t conjsN = totalCellsUsed / n;
    assert( totalCellsUsed % n  ==  0 );

    uint64_t midConj = conjsN/2;

    bool fstHasOneMoreConj = false;
    bool sndHasOneMoreConj = false;

    if ( conjsN % 2 != 0 )
    {
        if ( s->oddBias == 1  ||  s->oddBias == 0 &&
             common_randomNBiasedNonPeriodic(2) )
        {
            fstHasOneMoreConj = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                fprintf( s->outFile, 
                         "odd #: positive case has one more conjunction\n" );
            }
        }
        else
        {
            sndHasOneMoreConj = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                fprintf( s->outFile, 
                         "odd #: negative case has one more conjunction\n" );
            }
        }
    }

    // The start of the conclusion of a negative answer to the upper question,
    // including the possiblity that g0=R.
    uint64_t midStart = ( midConj + fstHasOneMoreConj ) * n;
    
    // Base number of conjunctions in a question, not including g0=R 
    // possibilities, and not counting fstHas or sndHas.
    uint64_t qConjs = conjsN / 2 - poss0R0;


    // Count randoms in conclusions at g1 for positive case, and g2 for
    // negative case. We'll try to minimize those, later.
    //   Random starting conjunctions will be ignored.
    uint64_t g1Rs = countRs( h, poss0R0 * n + 1, 
                             qConjs + fstHasOneMoreConj);
    uint64_t g2Rs = countRs( h, midStart + 2, 
                             qConjs + sndHasOneMoreConj );

    // Print info.
    if ( hi->settings->verbosityVector & HardVerbosity_printAll )
    {
        /*
        if ( fstHasOneMoreConj )
        { 
            fprintf( s->outFile, 
                     "split so that q has one more conjunction\n" );
       
        }

        if ( sndHasOneMoreConj )
        { 
            fprintf( s->outFile, 
                     "split so that -q has one more conjunction\n" );
       
        }
        */

        fprintf( s->outFile, "randoms at g1 for  q (not counting g0=R): %lu\n",
                             g1Rs );
        fprintf( s->outFile, "randoms at g2 for -q (not counting g0=R): %lu\n",
                             g2Rs );
    }


    // Try to swap conjunctions.
    if ( s->doSwaps > 0 )
    {
        swapConjunctions( h, poss0R0 * n, qConjs + fstHasOneMoreConj,
                             midStart, qConjs + sndHasOneMoreConj );

        if ( s->doSwaps > 1 )
        {
            swapConjunctions3a( h, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                   midStart, qConjs + sndHasOneMoreConj );
        }

        if ( s->doSwaps > 2 )
        {
            swapConjunctions3b( h, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                    midStart, qConjs + sndHasOneMoreConj );
        }                             

        if ( s->doSwaps > 3 )
        {
            swapConjunctions0111( h, 1, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                     2, midStart,    qConjs + sndHasOneMoreConj );
        }                             

        if ( s->doSwaps > 4 )
        {
            swapConjunctions1101( h, 1, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                     2, midStart,    qConjs + sndHasOneMoreConj );
        }                             

        
        // Re-count randoms in conclusions at g1 for positive case, and g2 for
        // negative case, after the minimization.
        //   Random starting conjunctions will be ignored.
        g1Rs = countRs( h, poss0R0 * n + 1, 
                           qConjs + fstHasOneMoreConj );
        g2Rs = countRs( h, midStart + 2, 
                           qConjs + sndHasOneMoreConj );

        // Print info.
        if ( hi->settings->verbosityVector & HardVerbosity_printAll )
        {
            fprintf( s->outFile, "\nafter swapping (not printing g0=R "
                                 "conjunctions):\n" );

            // For knowing when to print ---.
            // iMid is the start of the conjunction where neg half begins.
            uint64_t iMid = ( ( ( totalCellsUsed - n*poss0R0 )  -
                                n*poss0R0 )  /  n  /  2  +
                              fstHasOneMoreConj )  *  n  +  n * poss0R0;

            for ( uint64_t i = n * poss0R0; 
                  i != totalCellsUsed - n*poss0R0; )
            {
                printGod( godVal( g[godSlot(i)], godCell(i) ), s->outFile );

                i++;

                if ( i % n == 0 )
                {
                    fputc( '\n', s->outFile );

                    if ( i == iMid )
                    {
                        for ( uint8_t j = 0; j != n; j++ )
                        {
                            fputc( '-', s->outFile );
                        }

                        fputc( '\n', s->outFile );
                    }
                }
            }

            fputc( '\n', s->outFile );

            fprintf( s->outFile, "randoms at g1 for  q (not counting g0=R): %lu\n",
                                 g1Rs );
            fprintf( s->outFile, "randoms at g2 for -q (not counting g0=R): %lu\n",
                                 g2Rs );
        }


        // Do "non-improving" swaps, to balance sides.
        if ( s->doSwaps > 6  ||  s->doSwaps == 6 && g1Rs != 0 && g2Rs != 0 )
        {
            if ( g1Rs + 1 < g2Rs )
            {
                swapConjunctions2b( h, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                       midStart, qConjs + sndHasOneMoreConj,
                                       (g2Rs-g1Rs) / 2 );
            }
            else if ( g2Rs + 1 < g1Rs )
            {
                swapConjunctions2a( h, poss0R0 * n, qConjs + fstHasOneMoreConj,
                                       midStart, qConjs + sndHasOneMoreConj,
                                       (g1Rs-g2Rs) / 2 );
            }

            if ( g1Rs + 1 < g2Rs  ||  g2Rs + 1 < g1Rs )
            {
                // Re-count randoms in conclusions at g1 for positive case,
                // and g2 for negative case, after the balancing.
                //   Random starting conjunctions will be ignored.
                g1Rs = countRs( h, poss0R0 * n + 1, 
                                qConjs + fstHasOneMoreConj);
                g2Rs = countRs( h, midStart + 2, 
                                qConjs + sndHasOneMoreConj );

                // Print info.
                if ( hi->settings->verbosityVector & HardVerbosity_printAll )
                {
                    fprintf( s->outFile, "\nafter balancing (not printing R "
                                         "starting conjunctions):\n" );

                    // For knowing when to print ---.
                    // iMid is the conjunction where neg half begins.
                    uint64_t iMid = ( ( ( totalCellsUsed - n*poss0R0 )  -
                                        n*poss0R0 )  /  n  /  2  +
                                      fstHasOneMoreConj )  *  n  +
                                    n * poss0R0;

                    for ( uint64_t i = n * poss0R0; 
                          i != totalCellsUsed - n*poss0R0; )
                    {
                        printGod( godVal( g[godSlot(i)], godCell(i) ),
                                  s->outFile );

                        i++;

                        if ( i % n == 0 )
                        {
                            fputc( '\n', s->outFile );

                            if ( i == iMid )
                            {
                                for ( uint8_t j = 0; j != n; j++ )
                                {
                                    fputc( '-', s->outFile );
                                }

                                fputc( '\n', s->outFile );
                            }                            
                        }
                    }

                    fputc( '\n', s->outFile );

                    fprintf( s->outFile,
                             "randoms at g1 for  q (not counting g0=R): %lu\n",
                             g1Rs );
                    fprintf( s->outFile,
                             "randoms at g2 for -q (not counting g0=R): %lu\n",
                             g2Rs );
                }
            }
        }                             
    }


    // Add randoms in g0=R. Maybe this could have been done already
    // at the start. Maybe waiting is just confusing, with no gain.
    g1Rs += countRs( h, 0+1, poss0R0 );
    g2Rs += countRs( h, 0+2, poss0R0 );


    // Add that we are questioning g0, before the recursive calls.
    h->qPath[0] = 0;


    // Save state in case it becomes promising, and later deteriorate,
    // so that the promising state can be recreated, and the deterioration
    // undone. In fact, we'll skip this approach, and instead undo sub-
    // searches.

    // The seed before the positive call.
    //uint64_t posSeed = common_currentSeed();


    // State before positive call.
    uint64_t baseSubResN;
    double baseSubResSum;
    double baseAbortLeeway;

    // Counts how many active marks there are (this call). 
    uint8_t markCounter = 0;

    // Mark and save state, if there are randoms.
    if ( g1Rs != 0  &&  s->lvlReps[0] != 0 )
    {
        if ( conjHash_markState(h) )
        { 
            return Hard_outOfMemory;
        }

        markCounter++;

        baseSubResN = h->subresultsFound;
        baseSubResSum = h->subresSum;

        //uint64_t posCatchAbortsN = h->catchAbortsN;
        baseAbortLeeway = h->abortLeeway;    
        //posAbortLeewayDecrement = h->abortLeewayDecrement;
    }

    // Handle the positive case.
    uint8_t result = fnd1( hi, 0, midStart, 1, g1Rs, 1 );

    if ( result == Hard_outOfMemory )
    {
        return Hard_outOfMemory;
    }

    if ( result == Hard_aborted )
    {
        if ( markCounter != 0 )
        {
            assert( markCounter == 1 );

            conjHash_removeLastMark(h);
        }

        return Hard_aborted;
    }

    // True iff state is marked, i.e. a mark precedes all paths.
    bool stateIsMarked = false;


    // Repeat and keep best try.
    if ( g1Rs != 0  &&  s->lvlReps[0] != 0 )
    {
        // Save best state.
        uint64_t bestSubResN = h->subresultsFound;
        double bestSubResSum = h->subresSum;
        double bestAbortLeeway = h->abortLeeway;

        if ( conjHash_markState(h) )
        { 
            return Hard_outOfMemory;
        }

        markCounter++;

        for ( uint16_t i = 0; i != s->lvlReps[0]; i++)
        {
            // Reset state.
            h->subresultsFound = baseSubResN;
            h->subresSum = baseSubResSum;
            h->abortLeeway = baseAbortLeeway;

            // Do we want to shuffle??

            result = fnd1( hi, 0, midStart, 1, g1Rs, 1 );

            if ( result == Hard_outOfMemory )
            {
                return Hard_outOfMemory;
            }

            if ( result != Hard_aborted  &&
                 (double)h->subresSum / h->subresultsFound  <
                 (double)bestSubResSum / bestSubResN )
            {
                conjHash_undo2(h);

                if ( conjHash_markState(h) )  // should be done in undo2!!vvv
                { 
                    return Hard_outOfMemory;
                }

                bestSubResN = h->subresultsFound;
                bestSubResSum = h->subresSum;
                bestAbortLeeway = h->abortLeeway;
            }
            else
            {
                conjHash_undo(h);
            }
        }        

        // Update state.
        h->subresultsFound = bestSubResN;
        h->subresSum = bestSubResSum;
        h->abortLeeway = bestAbortLeeway;

        //conjHash_removeLastMark(h);
        stateIsMarked = true;
    }


    // Update best estimates.
    if ( (double)(h->subresSum) / h->subresultsFound  < 
         h->bestPositiveEstimates[0] )
    {
        h->bestPositiveEstimates[0] = (double)(h->subresSum) / 
                                      h->subresultsFound;
    } 

    // Save state.
    //uint64_t posCatchAbortsN = h->catchAbortsN;
    baseAbortLeeway = h->abortLeeway;    
    //posAbortLeewayDecrement = h->abortLeewayDecrement;
    baseSubResN = h->subresultsFound;
    baseSubResSum = h->subresSum;

    // Check if state is promising.

    bool stateIsPromising = false;

    //double posAbortLeeway;    
    //double posAbortLeewayDecrement;
    //uint64_t posSubresultsFound;
    //uint64_t posSubresSum;

    // The estimates are, now, decent. We should maybe use the global
    // upper bound here, instead of bestPositiveEstimates[0]?? Let's 
    // do it as an option:
    if ( s->globalBound )
    {
        if ( (double)(h->subresSum) / h->subresultsFound  <
            h->upperBound * s->resumeAbortedLeeway )
        {
            stateIsPromising = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                fprintf( s->outFile, "\nstate is promising: estimated result:"
                                    " %f\n",
                        (double)(h->subresSum) / h->subresultsFound );
            }
        }
    }
    else
    {    
        if ( (double)(h->subresSum) / h->subresultsFound  <
            h->bestPositiveEstimates[0] * s->resumeAbortedLeeway )
        {
            stateIsPromising = true;

            // Print info.
            if ( hi->settings->verbosityVector & HardVerbosity_printAll )
            {
                fprintf( s->outFile, "\nstate is promising: estimated result:"
                                    " %f\n",
                        (double)(h->subresSum) / h->subresultsFound );
            }
        }
    }

    // Set up possibility to reset the hash table. How?
    // We'll mark paths to know where we are at, and then
    // reset to latest/top marks if we want to undo a sub-search.
    //   We'll do it for the whole hash table; then we don't have to
    // hash. This works well here on the top level. Further below
    // we might have to only mark parts.
    //   We'll keep states marked if it could be useful (and is safe), even
    // if it turns out that it wasn't used in the end. Then there might be
    // mark leftovers at the end, but we'll handle that then if that's
    // the case. (Marks have no aftermath effect anyway since their 
    // probability is "infinitesimal" (1/2^256).)
    //   We'll only mark the state if needed.
    if ( !stateIsMarked  &&  g2Rs != 0  &&  s->lvlReps[0] != 0  ||
         !stateIsMarked  &&  g2Rs != 0  &&  stateIsPromising  &&
           h->catchAbortsN != 0 )
    {
        if ( conjHash_markState( h ) )
        { 
            return Hard_outOfMemory;
        }

        stateIsMarked = true;
        markCounter++;
    }

    // Handle the negative case.

    result = fnd1( hi, midStart, totalCellsUsed, 2, g2Rs, 1 );


    // Catch aborts.
    while ( stateIsPromising  &&  result == Hard_aborted  &&
            g2Rs != 0  &&  h->catchAbortsN != 0 )
    {
        h->catchAbortsN--;

        // Print info.
        if ( hi->settings->verbosityVector & HardVerbosity_printAll )
        {
            fprintf( s->outFile, "abort caught, retrying...\n" );
        }

        // Recreate state before last call.

        // Undo latest activity on the hash table.
        conjHash_undo( h );

        //common_srand(posSeed);

        h->abortLeeway = baseAbortLeeway;    
        //h->abortLeewayDecrement = posAbortLeewayDecrement;
        h->subresultsFound = baseSubResN;
        h->subresSum = baseSubResSum;

        // Get another seed. Or make something different. How?
        // Some variables are different. Maybe use them? Time might
        // not be different? If not enough time has passed.
        // This is not the approach taken any longer.

        // Should we shuffle the conjunctions again? They have been
        // scrambled a lot since start of last try already though.

        // Try the negative case again.
        result = fnd1( hi, midStart, totalCellsUsed, 2, g2Rs, 1 );
    }              

    stateIsMarked = false;

    if ( result == Hard_outOfMemory )
    {
        return Hard_outOfMemory;
    }

    if ( result == Hard_aborted )
    {
        // For the general case, fnd1, here we must remove recent marks.
        for ( ; markCounter != 0; markCounter-- )
        {
            conjHash_removeLastMark(h);
        }

        return Hard_aborted;
    }


    // Repeat and keep best try.
    if ( g2Rs != 0  &&  s->lvlReps[0] != 0 )
    {
        // Save best state.
        uint64_t bestSubResN = h->subresultsFound;
        double bestSubResSum = h->subresSum;
        double bestAbortLeeway = h->abortLeeway;

        if ( !stateIsMarked )
        {
            if ( conjHash_markState(h) )
            { 
                return Hard_outOfMemory;
            }

            markCounter++;
        }

        for ( uint16_t i = 0; i != s->lvlReps[0]; i++)
        {
            // Reset state.
            h->subresultsFound = baseSubResN;
            h->subresSum = baseSubResSum;
            h->abortLeeway = baseAbortLeeway;

            // Do we want to shuffle??

            result = fnd1( hi, midStart, totalCellsUsed, 2, g2Rs, 1 );

            if ( result == Hard_outOfMemory )
            {
                return Hard_outOfMemory;
            }

            if ( result != Hard_aborted  &&
                 (double)h->subresSum / h->subresultsFound  <
                 (double)bestSubResSum / bestSubResN )
            {
                conjHash_undo2(h);

                if ( conjHash_markState(h) )  // should be done in undo2!!vvv
                { 
                    return Hard_outOfMemory;
                }

                bestSubResN = h->subresultsFound;
                bestSubResSum = h->subresSum;
                bestAbortLeeway = h->abortLeeway;
            }
            else
            {
                conjHash_undo(h);
            }
        }        

        // Update state.
        h->subresultsFound = bestSubResN;
        h->subresSum = bestSubResSum;
        h->abortLeeway = bestAbortLeeway;

        //conjHash_removeLastMark(h);
        stateIsMarked = true;
    }

    // For the general case, fnd1, here we must remove recent marks.
    for ( ; markCounter != 0; markCounter-- )
    {
        conjHash_removeLastMark(h);
    }


    return result;
}



/*
// Finds optimal questions.
static bool solve( HardInstance * hi )
{


    return false;
}
*/



// Solves the generalization of "the hardest puzzle ever".
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_resultFound
// otherwise.
//   The algorithm will be derived from the paper
// 'How to Solve "The Hardest Logic Puzzle Ever" and Its
// Generalization'.
uint8_t hard_solve( HardInstance * hi )
{
    Settings * s = hi->settings;
    Hard * h = hi->hard;

    uint8_t retVal;  // The return value of searches.
    double bestResult = DBL_MAX;  // The best result found so far.
    uint64_t nrOfAborts = 0;  // Number of abort results.
    uint64_t seedForBestResult;  // The seed for the one best search. 
    uint64_t currentSeed = common_currentSeed();

    // We also have to save other settings to be able to recreate a
    // finding.
    double lvl0PosEstForBest;
    double currentBestLvl0PosEst = h->bestPositiveEstimates[0];
    double upperBoundForBest = h->upperBound;
    double abortLeewayStartForBest = s->abortLeewayStart;
    double abortLeewayEndForBest = s->abortLeewayEnd;

    // We'll search at least one --- besides, 'iterate 0 times' means
    // one search, I guess.
    /*
    if ( s->iterate == 0 )
    {
        return Hard_aborted;
    }
    */

    if Unlikely( h->godsN == 1 )
    {
        if ( s->verbosityVector & HardVerbosity_printResult )
        {
            fprintf( s->outFile, "There is only one god.\n" );
        }

        return Hard_aborted;  // ??
    }

    retVal = find1(hi);

    if ( retVal == Hard_outOfMemory )
    {
        return Hard_outOfMemory;
    }

    if ( retVal == Hard_resultFound )
    {
        bestResult = conjHash_questionAvg(hi->hard);
        seedForBestResult = currentSeed;
        lvl0PosEstForBest = currentBestLvl0PosEst;

        if ( bestResult < h->upperBound )
        {
            h->upperBound = bestResult;
        }

        // Print result.
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( s->outFile,
                     "\nResult after one search: average number of questions "
                     "asked to solve the problem: %3.*f\n", 
                     s->precision, bestResult );

            // Print best estimates.
            for ( uint8_t n = 0; n != s->maxCatchDepth; n++ )
            {
                fprintf( s->outFile,
                        "Best estimated result for the positive case, "
                        "depth %u: %f\n", 
                        n, h->bestPositiveEstimates[n] );
            }

            // Print estimate.
            fprintf( s->outFile, "Estimated result: %f\n", 
                     (double)(h->subresSum) / h->subresultsFound);

            fprintf( s->outFile, "number of aborts caught: %lu\n",
                     s->catchAbortsN - h->catchAbortsN );

            fputc( '\n', s->outFile );
        }

        // See if the bound is an absolute improvement, or a replication.
        if ( bestResult + BoundsFilePrecision < s->upperBoundInFile  &&
             s->updateBoundsFile )
        {
            readBounds_write( hi, bestResult, seedForBestResult, 
                              upperBoundForBest );
        }
        else if ( bestResult - BoundsFilePrecision < s->upperBoundInFile  &&
                  s->noteReplications >= 1  &&  
                  seedForBestResult != s->boundsFileSeed )
        {
            readBounds_noteRep( hi, seedForBestResult, upperBoundForBest );
        }
    }
    else
    {
        nrOfAborts++;

        // Print result.
        if ( s->verbosityVector & HardVerbosity_printInfo )
        {
            fprintf( s->outFile,
                     "\nFirst search aborted.\n\n" );
        }
    }

    
    // Repeat search.

    // An exponential factor to change abort leeways. It increases 
    // exponentially when situation is bad. And resets when
    // situation is fixed.
    //   This type of problem --- how to home in on good abort values ---
    // ought to be known. Something like "inverse" of "exponential decay".
    // Maybe ask ai, or someone, when this implementation is done.
    //   This approach istn't used any more.
    //double changeFactor = 1; 

    // Abort heuristic variables.
    //   We'll do searches until a non-aborted search happen,
    // and maybe an aborted one too. Then we calculate abort probabilities, 
    // and adjust values depending on how far away we are, and perhaps certainty
    // intervals.

    // The abort rate, with exponential decay. For the abort heuristics.
    //double expDecayAbortRate = 0.5;  // ??  // Not used any more.

    uint32_t aborts = 0;  // Number of aborts since abort heuristic batch start.
    uint32_t solutions = 0;  // Number of non-aborts since abort heuristic batch start.


    for ( uint32_t reps = 0; reps != s->iterate; reps++ )
    {
        // Reset h to prepare for a repeated call to find1.

        h->cellsUsed = 0;
        h->slotsUsed = 0;
        h->godsInConj = 0;
        h->godsInPrefix = 0;
        h->subresultsFound = 0;
        h->subresSum = 0;
        h->abortLeeway = s->abortLeewayStart;
        h->catchAbortsN = s->catchAbortsN;


        // For the hash table, we'll just delete it and remake it from
        // scratch. This is a bit wasteful, but not much, and easier.
        conjHash_delete(h->ht);
        h->ht = conjHash_new(h);
        
        if ( h->ht == NULL )
        {
            fprintf( stderr, "\nError: not enough memory.\n\n" );

            return Hard_outOfMemory;
        }

        // Save the state.
        currentSeed = common_currentSeed();
        currentBestLvl0PosEst = h->bestPositiveEstimates[0];

        retVal = find1(hi);

        if ( retVal == Hard_outOfMemory )
        {
            return Hard_outOfMemory;
        }

        if ( retVal == Hard_resultFound )
        {
            double result = conjHash_questionAvg(hi->hard);

            if ( result < bestResult )
            {
                bestResult = result;
                seedForBestResult = currentSeed;
                lvl0PosEstForBest = currentBestLvl0PosEst;
                abortLeewayStartForBest = s->abortLeewayStart;
                abortLeewayEndForBest = s->abortLeewayEnd;

                if ( bestResult < h->upperBound )
                {
                    upperBoundForBest = h->upperBound;
                    h->upperBound = bestResult;
                }

                if ( s->verbosityVector & HardVerbosity_printInfo )
                {
                    fprintf( s->outFile,
                             "Result after %u searches: average number of questions "
                             "asked to solve the problem: %3.*f\n", 
                             reps+2, s->precision, bestResult );

                    // Print state before search so that it can succinctly
                    // be recreated (in case of an abort of the whole run;
                    // this info is also printed at the end).
                    if ( s->verbosityVector & HardVerbosity_printSeed )
                    {
                        if ( s->globalBound )
                        {
                            fprintf( s->outFile,
                                    "Start state: seed: %lu, upper bound (-u): %.*g\n", 
                                    seedForBestResult, 
                                    DBL_DECIMAL_DIG, upperBoundForBest );
                        }
                        else
                        {
                            fprintf( s->outFile,
                                    "Start state: seed: %lu, best lvl 0 pos est (-E): %.*g, upper bound (-u): %.*g\n", 
                                    seedForBestResult, DBL_DECIMAL_DIG, lvl0PosEstForBest,
                                    DBL_DECIMAL_DIG, upperBoundForBest );
                        }

                        fprintf( s->outFile,
                                "abort-leeway-start: %g\n", s->abortLeewayStart );

                        fprintf( s->outFile,
                                "abort-leeway-end: %g\n\n", s->abortLeewayEnd );
                    }

                    // Print best estimates.
                    for ( uint8_t n = 0; n != s->maxCatchDepth; n++ )
                    {
                        fprintf( s->outFile,
                                "Best estimated result for the positive case, "
                                "depth %u: %f\n", 
                                n, h->bestPositiveEstimates[n] );
                    }

                    // Print estimate.
                    fprintf( s->outFile, "Estimated result: %f\n", 
                             (double)(h->subresSum) / h->subresultsFound);

                    fprintf( s->outFile, "number of aborts caught: %lu\n",
                             s->catchAbortsN - h->catchAbortsN );

                     fputc( '\n', s->outFile );
                }

                // See if the bound is an absolute improvement, or a replication.
                if ( bestResult + BoundsFilePrecision < s->upperBoundInFile  &&
                     s->updateBoundsFile )
                {
                    readBounds_write( hi, bestResult, seedForBestResult,
                                      upperBoundForBest );
                }
                else if ( bestResult - BoundsFilePrecision < s->upperBoundInFile  &&
                          s->noteReplications >= 1  &&  
                          seedForBestResult != s->boundsFileSeed )
                {
                    readBounds_noteRep( hi, seedForBestResult, upperBoundForBest );
                }
            }
            else if ( result - BoundsFilePrecision < s->upperBoundInFile  &&
                      s->noteReplications >= 1  &&  
                      seedForBestResult != s->boundsFileSeed )
            {   // Note replication.
                readBounds_noteRep( hi, seedForBestResult, upperBoundForBest );
            }
            
            solutions++;
        }
        else
        {
            nrOfAborts++;
            aborts++;

            // Print result, occasionally.
            if ( ( s->iterate >= 8  &&  reps % ( s->iterate / 8 ) == 0  ||
                   s->iterate < 8 )  &&
                 s->verbosityVector & HardVerbosity_printInfo )
            {
                fprintf( s->outFile,
                         "Search number %u aborted.\n\n", reps+2 );
            }
        }

        // Update abort heuristics.
        if ( s->abortPromilleGoal <= 1000 )
        {
            uint32_t sampleSize = aborts + solutions;

            if ( sampleSize >= 2 * s->minSampleSize  ||
                 sampleSize >= s->minSampleSize  &&  
                 aborts > 0  &&  solutions > 0 )  // ??
            {
                // Calculate confidence intervals, CIs.
                // CI = `p` ± 1.96 * √(`p` * (1 - `p`) / `n`)
                //   Ideally, the sample size shoulbe be >= 
                // 5 / (1 - abort_goal), assuming abort_goal >= 50%.
                double p = (double)(aborts) / sampleSize;
                double se = sqrt( ( p * ( 1 - p ) ) / sampleSize );
                double moe = s->ciz * se;

                if ( 1000 * p  >  s->abortPromilleGoal )
                {
                    double change = 
                        s->changeFactor  * 
                        ( 1 + p - (double)s->abortPromilleGoal/1000 )  *
                        ( 1 + 1 - moe )  *  // ??
                        s->abortLeewayChange;
                    s->abortLeewayStart += change;
                    s->abortLeewayEnd   += change;
                }
                else if ( 1000 * p  <  s->abortPromilleGoal )
                {
                    double change = 
                        s->changeFactor  *
                        ( 1 + (double)s->abortPromilleGoal/1000 - p )  *
                        ( 1 + 1 - moe )  *  // ??
                        s->abortLeewayChange;
                    s->abortLeewayStart -= change;
                    s->abortLeewayEnd   -= change;
                }

                // Reset.
                aborts = 0;
                solutions = 0;
            }
        }
    }


    if ( s->verbosityVector & HardVerbosity_printInfo )
    {
        fprintf( s->outFile,
                 "Searches aborted: %f %%\n", 
                 100 * (double)(nrOfAborts) / ( s->iterate + 1 ) );

        fprintf( s->outFile,
                 "abort-leeway-start: %g\n", s->abortLeewayStart );

        fprintf( s->outFile,
                 "abort-leeway-end: %g\n\n", s->abortLeewayEnd );

        // Print best estimates.
        for ( uint8_t n = 0; n != s->maxCatchDepth; n++ )
        {
            fprintf( s->outFile,
                     "Best estimated result for the positive case, "
                     "depth %u: %f\n", 
                     n, h->bestPositiveEstimates[n] );
        }
    }
  

    if ( bestResult != DBL_MAX )
    {
        // Print result.

        if ( s->verbosityVector & HardVerbosity_printSeed )
        {
            fprintf( s->outFile, "Start state for the one search that found "
                     "the best result:\n");

            fprintf( s->outFile, "  seed: %lu\n", seedForBestResult );

            fprintf( s->outFile,
                     "  abort-leeway-start: %g\n", abortLeewayStartForBest );

            fprintf( s->outFile,
                     "  abort-leeway-end: %g\n", abortLeewayEndForBest );
        }

        if ( s->verbosityVector & HardVerbosity_printSeed  &&
             !s->globalBound )
        {
            fprintf( s->outFile,
                     "  best lvl 0 positive estimate (-E): %.*g\n", 
                     DBL_DECIMAL_DIG, lvl0PosEstForBest );
        }

        if ( s->verbosityVector & HardVerbosity_printSeed )
        {
            fprintf( s->outFile,
                     "  upper bound (-u): %.*g\n\n",
                     DBL_DECIMAL_DIG, upperBoundForBest );
        }

        if ( s->verbosityVector & HardVerbosity_printResult )
        {
            fprintf( s->outFile,
                     "Best result found: average number of questions "
                     "asked to solve the problem: %3.*f\n\n", 
                     s->precision, bestResult );
        }
        
        return Hard_resultFound;
    }
    else
    {
        // Print result.
        if ( s->verbosityVector & HardVerbosity_printResult )
        {
            fprintf( s->outFile,
                     "All searches were aborted.\n\n" ); 
        }

        return Hard_aborted;
    }
}



/*
  gprof:
  newer nearer top:

0.15.6  
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 22.83     22.77    22.77 581699143     0.00     0.00  swapConjs
 16.51     39.23    16.46 1235604572     0.00     0.00  countRs
 14.02     53.22    13.98 285089868     0.00     0.00  swapConjunctions0111
  8.28     61.48     8.26 285089865     0.00     0.00  swapConjunctionsG
  7.17     68.62     7.15 285089865     0.00     0.00  swapConjunctionsG3a
  6.91     75.52     6.89 285089865     0.00     0.00  swapConjunctionsG3b
  5.64     81.14     5.62 285089868     0.00     0.00  swapConjunctions1101
  4.43     85.56     4.42 322556271     0.00     0.00  hashFNV
  3.52     89.07     3.51  1966137     0.00     0.00  count2RsQuad
  2.66     91.72     2.65 264114713     0.00     0.00  equalConj
  1.87     93.59     1.86 106344843     0.00     0.00  randomsAsked
  1.39     94.98     1.39  1835947     0.00     0.00  conjHash_removeLastLocalMark
  1.21     96.19     1.21  2124315     0.00     0.00  conjHash_markStateLocally
  0.93     97.12     0.93   288368     0.00     0.00  conjHash_undo2Locally
  0.56     97.68     0.56 91428686     0.00     0.00  conjHash_addIf
  0.48     98.16     0.48   694665     0.00     0.00  conjHash_undoLocally
  0.45     98.61     0.45 106344843     0.00     0.00  conjHash_lookupHL

0.15.6  
  Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 22.36     30.39    30.39 815392874     0.00     0.00  swapConjs
 16.80     53.22    22.83 1768299868     0.00     0.00  countRs
 14.25     72.58    19.37 408003178     0.00     0.00  swapConjunctions0111
  8.64     84.33    11.74 408003175     0.00     0.00  swapConjunctionsG
  7.20     94.12     9.79 408003175     0.00     0.00  swapConjunctionsG3b
  6.84    103.42     9.29 408003175     0.00     0.00  swapConjunctionsG3a
  5.76    111.24     7.83 408003178     0.00     0.00  swapConjunctions1101
  4.18    116.92     5.68 457735409     0.00     0.00  hashFNV
  4.00    122.36     5.44  2813815     0.00     0.00  count2RsQuad
  2.58    125.86     3.50 373717884     0.00     0.00  equalConj
  1.71    128.18     2.32 150922618     0.00     0.00  randomsAsked
  1.47    130.18     2.00  2627653     0.00     0.00  conjHash_removeLastLocalMark
  0.97    131.50     1.32  3037806     0.00     0.00  conjHash_markStateLocally
  0.60    132.32     0.82   410153     0.00     0.00  conjHash_undo2Locally
  0.47    132.97     0.65 130045243     0.00     0.00  conjHash_addIf
  0.47    133.60     0.64   996685     0.00     0.00  conjHash_undoLocally
  0.46    134.22     0.62 150922618     0.00     0.00  conjHash_lookupHL
  0.41    134.78     0.56       12     0.05    11.30  fnd1
  0.28    135.17     0.39 20877375     0.00     0.00  conjHash_add


This is with -b 1 (and defaults are somewhat fixed, now):

    %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 32.87     18.04    18.04 491168766     0.00     0.00  countRs
 23.88     31.14    13.11 536795641     0.00     0.00  swapConjs
  9.49     36.35     5.21 268517705     0.00     0.00  randomsAsked
  4.81     38.99     2.64 293719663     0.00     0.00  hashFNV
  4.10     41.24     2.25 323314902     0.00     0.00  equalConj
  2.89     42.83     1.58    29569     0.00     0.00  fnd1
  2.79     44.36     1.53 12210357     0.00     0.00  count2Rs
  2.30     45.62     1.26 268517983     0.00     0.00  conjHash_lookupHL
  1.71     46.56     0.94 227449467     0.00     0.00  conjHash_addIf
  1.48     47.37     0.81 12210357     0.00     0.00  rSort
  1.42     48.15     0.78 452114143     0.00     0.00  common_randomN
  1.26     48.84     0.69 12208581     0.00     0.00  rSortb
  1.09     49.44     0.60    15001     0.00     0.00  conjHash_initHash
  1.02     50.00     0.56 12210357     0.00     0.00  swapConjunctionsG
  0.91     50.50     0.50    14556     0.00     0.00  conjHash_questionAvg
  0.84     50.96     0.46 12210357     0.00     0.00  swapConjunctionsG3b
  0.82     51.41     0.45 12225358     0.00     0.00  swapConjunctions0111
  0.82     51.86     0.45 12210357     0.00     0.00  swapConjunctionsG3a
  0.74     52.27     0.41 24465717     0.00     0.00  shuffleConjunctions
  0.73     52.66     0.40 458358307     0.00     0.00  common_rand
  0.73     53.06     0.40 12225358     0.00     0.00  swapConjunctions1101
  0.56     53.37     0.30 33587239     0.00     0.00  copyGods
  0.45     53.62     0.24 41068516     0.00     0.00  conjHash_add
  0.41     53.84     0.23 104931995     0.00     0.00  setGodCP
  0.40     54.06     0.22 61440000     0.00     0.00  freeHashList
  0.35     54.25     0.19 104946996     0.00     0.00  addToPrefix
  0.27     54.40     0.15 25200000     0.00     0.00  freePathList
  0.20     54.51     0.11                             _init
  0.19     54.62     0.10    30002     0.00     0.00  setPoss
  0.13     54.69     0.07  6244164     0.00     0.00  common_randomNBiased
  0.09     54.73     0.05   336764     0.00     0.00  swapConjunctionsG2b

   
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 29.03      2.92     2.92 150423936     0.00     0.00  countRs
 15.71      4.50     1.58 62079747     0.00     0.00  swapConjs
 13.67      5.88     1.38 61045175     0.00     0.00  randomsAsked
  5.86      6.46     0.59    28000     0.00     0.00  fnd1
  5.62      7.03     0.56 71047466     0.00     0.00  equalConj
  5.17      7.55     0.52  4670159     0.00     0.00  count2Rs
  4.87      8.04     0.49 63985175     0.00     0.00  hashFNV
  2.98      8.34     0.30 56122945     0.00     0.00  conjHash_addIf
  2.49      8.59     0.25 61045175     0.00     0.00  conjHash_lookupHL
  1.79      8.77     0.18  9382318     0.00     0.00  shuffleConjunctions
  1.79      8.95     0.18  4670159     0.00     0.00  rSort
  1.79      9.13     0.18  4670159     0.00     0.00  rSortb
  1.59      9.29     0.16  4670159     0.00     0.00  swapConjunctionsG
  1.19      9.41     0.12  4670159     0.00     0.00  swapConjunctionsG3b
  0.80      9.49     0.08  4684159     0.00     0.00  swapConjunctions0111
  0.60      9.55     0.06  4670159     0.00     0.00  swapConjunctionsG3a
  0.60      9.61     0.06    14000     0.00     0.00  conjHash_questionAvg
  0.50      9.66     0.05    28000     0.00     0.00  setPoss
  0.50      9.71     0.05    14000     0.00     0.00  conjHash_initHash

*/



// Frees and closes hi.
void hard_free( HardInstance * hi )
{
    Settings * s = hi->settings;
    Hard * h = hi->hard;


    if ( s->outFileName != NULL )
    {
        if ( s->outFile != stdout  &&  s->outFile != stderr )
        {
            fclose(s->outFile);
        }

        free(s->outFileName);
    }


    free(h->gods);

    free(s);
    free(h);
    free(hi);
}

