// Module for conjunction hashing. FNV based.


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
    vallst@gmail.com.

    All software distributed under the License is provided in the hope
    that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE. See the License for more details.
*/


#include "conjHash.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "hard.h"
#include "common.h"
#include "compilerMacros.h"


// A mask for n bits.
#define mask(n) ( ( 1u << (n) ) - 1 )

// Magic constants:
#define FNV_prime32  16777619
#define FNV_prime64  ((uint64_t)1099511628211)
#define FNV_prime    FNV_prime64
#define FNV_offset_basis32 ((uint64_t)2166136261)
#define FNV_offset_basis64 ((uint64_t)14695981039346656037)



#if 0
// Returns a hash value of c. The FNV-1 hash algorithm is used here.
//   2^logSize should be the size of the hash-table.
//   Reference: http://www.isthe.com/chongo/tech/comp/fnv/
//   cLen is the size of c, in number of bytes, not counting fstByte or lastByte.
//   fstByte and lastByte should be the first and last bytes of c, with bits
// not in the conjunction zeroed out; c should point to the second byte, the one
// after fstByte.
//   This assumes that the conjunction spans over at least 2 bytes. If that's
// not the case, some other function has to be used.
//   This won't work. It's not a function: the same conjunction can move around,
// and produce different values depending on where it starts in a byte. We'll
// do it on a cell level instead. (Conjunctions really should have been
// padded, and aligned.)
static inline uint64_t vvvhashFNV( uint8_t * c, unsigned int logSize, 
                                uint8_t fstByte, uint8_t lastByte, uint8_t cLen )
{
    // n will contain the accumulated hash value.
    uint64_t n = FNV_offset_basis64;

    n *= FNV_prime;
    n ^= fstByte;
    
    for ( ; cLen != 0; c++, cLen-- )
    {
        n *= FNV_prime;
        n ^= *c;
    }

    n *= FNV_prime;
    n ^= lastByte;

    return ( ( n >> logSize ) ^ n ) & mask(logSize);
}
#endif



// Returns a hash value of a conjunction c.
//   And adaptation of the FNV-1 hash algorithm is used.
//   2^logSize should be the size of the hash-table.
//   Reference: http://www.isthe.com/chongo/tech/comp/fnv/
//   c points to the start of the conjunction, in h->gods. 
//   AI suggested MurmurHash or xxHash as an alternativ to
// FNV-1. This function might be acceptable though.
// Collisions are maybe around 20%.
static inline uint64_t hashFNV( uint64_t c, Hard * h )
{
    God * g = h->gods;
    uint8_t godsN = h->godsN;
    uint8_t logSize = h->ht->logSize;

    // n will contain the accumulated hash value.
    uint64_t n = FNV_offset_basis64;
    

    // Handle whole bytes first.
    for ( uint8_t wholeBytesN = godsN / 4; wholeBytesN != 0; 
          wholeBytesN--, c += 4 )
    {
        // Do a single byte, 4 conjuncts.
        uint8_t byte = 0;  // The byte to be used to hash.
        for ( uint8_t i = 0; i != 4; i ++ )  // Opposite order from remainders.
        {
            // Get the c+i god
            uint8_t k = godV( g, c+i );
            k = k << i*2;
            byte = byte | k;
        }

        n *= FNV_prime;
        n ^= byte;
    }
    

    // Handle the remainder conjuncts.
    uint8_t byte = 0b11000000;  // The byte to be used to hash.
    for ( uint8_t remainder = godsN % 4; remainder != 0; )
    {
        remainder--;
        // Get the c+remainder god.
        uint8_t k = godV( g, c+remainder );
        k = k << remainder*2;
        byte = byte | k;
    }

    n *= FNV_prime;
    n ^= byte;  // Do we want to set most significant bits? To get better hash?
                // byte can be 0, here. Which is fine, I guess.


    return ( ( n >> logSize ) ^ n ) & mask(logSize);
}



// Frees the list.
static void freePathList( PathList * n )
{
    PathList * l;
    while ( n != NULL )
    {
        l = n;
        n = n->next;
        free(l);
    }
}


// Frees the list, including the paths.
static void freeHashList( HashList * n )
{
    HashList * l;
    while ( n != NULL )
    {
        l = n;
        n = n->next;
        free(l->conj);
        freePathList(l->path);
        free(l);
    }
}


void conjHash_delete( ConjHash * h )
{
    for ( HashList * * t = h->tab, * * tEnd = h->tab + ( 1u << h->logSize );
          t != tEnd; t++ )
    {
        freeHashList(*t);
    }

    free(h->tab);
    free(h);
}



// Returns true iff conjunction starting at i equals conjunction c.
//   i points into h->gods. c is separate, coming from e.g. the hash table.
static inline bool equalConj( uint64_t i, Hard * h, uint8_t * c )
{
    uint8_t n = h->godsN;
    God * g = h->gods;

    for ( uint64_t j = 0; j != n; j++ )
    {
        if ( godV( g, i+j )  !=  godV( c, j ) )
        {
            return false;
        }
    }

    return true;
}



// Returns the hash table array pointer for a conjunction.
HashList * * conjHash_bucket( uint64_t c, Hard * h )
{
    ConjHash * ht = h->ht;

    return ht->tab + hashFNV( c, h );
}


// Returns the HashList node of conjunction c. c should be an index pointing 
// into h->gods.
//   Returns NULL if no entry was found.
HashList * conjHash_lookupHL( uint64_t c, Hard * h )
{
    ConjHash * ht = h->ht;

    HashList * ps0 = (ht->tab)[hashFNV( c, h )];

    // See if c already is in the table.
    for ( HashList * ps = ps0; ps != NULL; ps = ps->next )
    {
        if ( equalConj( c, h, ps->conj ) )
        {
            return ps;
        }
    }

    return NULL;
}



PathList * conjHash_lookup( uint64_t c, Hard * h )
{
    ConjHash * ht = h->ht;

    HashList * * psPtr0 = ht->tab + hashFNV( c, h );
    // (The meta pointer isn't really used here any more.)

    // See if c already is in the table.
    for ( HashList * ps = *psPtr0; ps != NULL; ps = ps->next )
    {
        if ( equalConj( c, h, ps->conj ) )
        {
            return ps->path;
        }
    }

    return NULL;
}

#if 0
    // Add node.

    (h->nrOfEntries)++;

    HashList * bs = malloc( sizeof(HashList) );
    if ( bs == NULL )
    {
        return HT_error;
    }

    bs->path = vvv;

    bs->conj = malloc( (hrd->godsN)/4 + ( (hrd->godsN)%4 !=0 ) );
    if ( bs->conj == NULL )
    {
        return HT_error;
    }
 
    strcpy( bs->str, s );

    bs->next = *psPtr0;
    *psPtr0 = bs;
    

    // Check if the table needs to be doubled.
    if ( h->maxLoad < (double)(h->nrOfEntries) / ( 1u << h->logSize ) )
    {
        if ( conjHash_doubleSize(h) )
        {
            return bs->val;  // ??
        }
    }

    return bs->val;
}
#endif


#if 0
// We'll assume that c is in the table.
static HashList * conjHash_lookupFast( uint64_t c, Hard * h )
{
    ConjHash * ht = h->ht;

    HashList * hl = ht->tab[ hashFNV( c, h ) ];

    // See if c already is in the table.
    for ( ; hl != NULL; hl = hl->next )
    {
        if ( hl->next == NULL  ||  equalConj( c, h, hl->conj ) )
        {
            return hl;
        }
    }

    return NULL;
}
#endif



// Returns a NULL initialized size 2^logSize table, or NULL if the
// allocation fails.
static HashList * * newConjTable( unsigned int logSize )
{
    HashList * * table = malloc( sizeof( HashList * ) * ( 1u << logSize ) );

    if ( table == NULL )
    {
        return NULL;
    }

    for ( HashList * * t = table, * * tEnd = table + ( 1u << logSize );
          t != tEnd; t++ )
    {
        *t = NULL;
    }

    return table;
}


ConjHash * conjHash_new( Hard * h )
//unsigned int logSize, double maxLoad )
{
    ConjHash * ht = malloc( sizeof(ConjHash) );

    if ( ht == NULL )
    {
        return NULL;
    }

    ht->nrOfEntries = 0;
    ht->maxLoad = DefaultMaxLoad;
    ht->nrOfCollisions = 0;

    // Calculate an appropriate logSize.
    uint8_t logSize = ceil( log2( h->possN / ht->maxLoad + 1 ) );

    ht->logSize = logSize;

    ht->tab = newConjTable(logSize);

    if ( ht->tab == NULL )
    {
        free(ht);

        return NULL;
    }

    return ht;
}



// Adds node bs to bucket asPtr.
static inline void addConjNode( HashList * * asPtr, HashList * bs )
{
    bs->next = *asPtr;
    *asPtr = bs;
}



#if 0    
bool conjHash_doubleSize( ConjHash * h )
{
    HashList * * oldHT = h->tab;

    // Is the new size too big?
    if ( h->logSize+1 >= sizeof(unsigned int) * CHAR_BIT )
    {
        h->maxLoad += 0.1;

        return false;
    }

    h->tab = newConjTable( h->logSize+1 );

    if ( h->tab == NULL )
    {
        h->tab = oldHT;

        return true;
    }

    // Insert the formulas into the new table using the new size.

    h->nrOfCollisions = 0;
    h->logSize += 1;

    for ( HashList * * t = oldHT, * * tEnd = t + ( 1u << (h->logSize-1) );
          t != tEnd; t++ )
    {
        HashList * next;

        for ( HashList * as = *t; as != NULL; as = next )
        {
            next = as->next;
            addConjNode( h->tab + hashFNV( as->str, h->logSize ), as );
        }
    }

    free(oldHT);


    return false;
}
#endif



// Initializes the hash table. We'll put all conjunctions in it.
//   Returns true iff not enough memory could be allocated.
//   poss0R0 is the number of possibilities with starting R. (We should
// just get rid of poss0R0 altogether, everywhere. And use the fnd1 approach.)
bool conjHash_initHash( Hard * h, uint64_t poss0R0 )
{
    God * g = h->gods;
    ConjHash * ht = h->ht;
    God n = h->godsN;
    uint64_t conjs = h->possN;

    for ( uint64_t c = 0; c != conjs; c++ )
    {
        // Do a conjunction.

        // Create node for the conjunction.

        HashList * hl = malloc( sizeof(HashList) );

        if ( hl == NULL )
        {
            return true;
        }

        hl->path = NULL;

        hl->conj = malloc( n/God_GodsPerSlot + ( n%God_GodsPerSlot !=0 ) );

        if ( hl->conj == NULL )
        {
            return true;
        }

        // The bucket to use.
        HashList * * htPtr = conjHash_bucket( poss0R0*n + c*n, h );

        if ( *htPtr != NULL )
        {
            hl->next = *htPtr;

            ht->nrOfCollisions++;
        }
        else
        {
            hl->next = NULL;
        }

        *htPtr = hl;
        ht->nrOfEntries++;

        // Set conjunction. The last bits are not set (and hence random),
        // if n%4 != 0, which is fine.
        uint8_t * conj = hl->conj;
        for ( God i = 0; i != n; i++ )
        {
            God v = godV( g, poss0R0*n + c*n + i );
            setGod( conj, godSlot(i), godCell(i), v );
        }
    }

    assert( ht->nrOfEntries == h->possN );

    return false;
}



#if 0
void conjHash_print( Hard * h, FILE * f )
{
    ConjHash * ht = h->ht;
    uint64_t htSize = 1u << ht->logSize;

    assert( ht->nrOfEntries == h->possN );

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * as = (ht->tab)[k]; as != NULL; as = as->next )
        {

        }
    }
}
#endif



// Calculates the average number of questions asked.
double conjHash_questionAvg( Hard * h )
{
    ConjHash * ht = h->ht;
    uint64_t htSize = 1u << ht->logSize;

    assert( ht->nrOfEntries == h->possN );

    double sum = 0;  // Accumulated sum of questions.

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * as = (ht->tab)[k]; as != NULL; as = as->next )
        {
            // Do the disjunct.

            double disSum = 0;  // For calculating the disjunct value.
            //uint8_t ways = 0;   // The number of ways that the disjunct can
                                // be reached.
            #if AssertionLevel >= 2
            double prob = 0;    // The probabilities. Should add up to 1.
            #endif

            for ( PathList * path = as->path; path != NULL; path = path->next )
            {
                // Skip marks. (Not needed, really.)
                if ( path->rqs != RQsUndef )
                {
                    //ways++;
                    disSum += 1 / pow( 2, path->rqs ) * path->qs;

                    #if AssertionLevel >= 2
                    prob += 1 / pow( 2, path->rqs );
                    #endif
                }
            }

            sum += disSum;

            //assert2( prob == 1 );
            assert2( fabs( 1 - prob ) < 1e-9 );  // Is this a safe value?
        }
    }

    sum = sum / ht->nrOfEntries;

    return sum;
}



// Statistics of chain lengths of 0..maxLength-1 will be collected.
#define maxLength 32

void conjHash_stats( Hard * h, FILE * f )
{
    ConjHash * ht = h->ht;

    uint64_t htSize = 1u << ht->logSize;

    uint64_t chainLengths[maxLength] = { 0 };

    // Number of chains >= maxLength.
    uint64_t nrOfLongChains = 0;

    uint64_t nrOfEntriesInTable = 0;

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        unsigned int chainLength = 0;

        for ( HashList * as = (ht->tab)[k]; as != NULL; as = as->next )
        {
            //assert( conjHash_lookup( ht, as->str ) == as->val );  // Useful?

            nrOfEntriesInTable++;
            chainLength++;
        }

        // printf( "%u: %u\n", k, chainLength );

        if ( chainLength >= maxLength )
        {
            nrOfLongChains++;
        }
        else
        {
            chainLengths[chainLength]++;
        }
    }

    assert( nrOfEntriesInTable == ht->nrOfEntries );

    fprintf( f, "ht-size: %lu\n", htSize );
    fprintf( f, "more hash table stats, collisions "
                "and chain lengths:\n" );

    for ( unsigned int k = 0; k != maxLength; k++ )
    {
        if ( chainLengths[k] != 0 )
        {
            fprintf( f, "%u: %lu\n", k, chainLengths[k] );
        }
    }

    if ( nrOfLongChains != 0 )
    {
        fprintf( f, ">=%u: %lu\n", maxLength, nrOfLongChains );
    }

    fprintf( f, "load: %.3f\n", (double)(ht->nrOfEntries) / (1u<<ht->logSize) );
    fprintf( f, "number of entries: %lu\n\n", ht->nrOfEntries );
}



// Calculates the number of randoms that have been questioned,
// given local premise conjunction c. qs is the number of 
// questions asked.
static inline uint16_t randomsAsked( uint64_t c, Hard * h, uint8_t qs )
{
    uint16_t rqs = 0;

    GodsN * qPath = h->qPath;
    God * g = h->gods;

    // Loop through qPath and see how many are random, given the local premise c.
    for ( uint8_t k = 0; k != qs; k++ )
    {
        if ( godV( g, c + qPath[k] ) == randomGod )
        {
            rqs++;
        }
    }

    return rqs;
}



// Adds path. c is the conjunction. qs is the number of questions used
// to reach. qs0 is the length of qPath.
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_pathAdded (0)
// otherwise.
uint8_t conjHash_add( uint64_t c, uint8_t qs, HardInstance * hi, uint8_t qs0 )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    // Get node.
    HashList * hl = conjHash_lookupHL( c, h );

    if ( hl == NULL )
    {
        // This can't or shouldn't happen.
        fprintf( stderr, "\nError: conjunction not found in hash table.\n\n" );

        assert( false );

        return Hard_outOfMemory;
    }

    // Count random answer occurencies in questions path.
    uint8_t randomAnswersN = randomsAsked( c, h, qs0 );

    // Update leeway and heuristic numbers.

    h->abortLeeway = max( h->abortLeeway - h->abortLeewayDecrement,
                          s->abortLeewayEnd );

    uint64_t powRs = (uint64_t)1 << randomAnswersN;    

    if ( s->estimateHeuristic == 1 )
    {                          
        // We'll weigh the addition according to the probability,
        // 1/2^randomAnswersN.

        // We'll have to handle the case where h->subresultsFound is 0.    
        if Unlikely( h->subresultsFound == 0 )
        {
            // What to do here? This will bias the estimate a bit,
            // when randomAnswersN != 0. But without some base,
            // what can you do? However, always, or almost always(?),
            // randomAnswers should be 0 here, so this should be fine.
            assert( h->subresSum == 0 );
            h->subresSum = qs;
        }
        else
        {
            if ( powRs == 1 )
            {
                h->subresSum += qs; 
            }
            else
            {
                h->subresSum += (double)qs * (1+s->estWeight) / powRs  +  
                                ( ( h->subresSum / h->subresultsFound ) *
                                ( (double)(powRs-1-s->estWeight)/powRs) ); 
            }
        }
        h->subresultsFound++;
    }
    else if ( s->estimateHeuristic == 2  ||  powRs == 1  ||
              common_randomNBiased(powRs) == 0 )
    {
        h->subresSum += qs;
        h->subresultsFound++;
    }
    

    // See if we want to abort the search.
    if ( h->subresultsFound > s->dontAbortUntil  &&
         (double)(h->subresSum) / h->subresultsFound >
         h->upperBound * h->abortLeeway )
    {
        // Print info.
        if ( s->verbosityVector & HardVerbosity_printAll )
        {
            indent( s->indent * qs0, s->outFile );
            fprintf( s->outFile, 
                     "Unpromising subsearch aborted. Estimated |Qs|: %f\n",
                     (double)(h->subresSum) / h->subresultsFound );
        }

        return Hard_aborted;
    }

    // Make new path node.
    PathList * pl = malloc( sizeof(PathList) );

    if ( pl == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return Hard_outOfMemory;
    }

    pl->next = hl->path;
    pl->qs = qs;
    pl->rqs = randomAnswersN;

    // Insert node first.
    hl->path = pl;

    // Print info.
    if ( s->verbosityVector & HardVerbosity_printAll )
    {
        // Do we want to use qs instead of qs0 for tabs? Probably not.
        indent( s->indent * qs0, s->outFile );
        fprintf( s->outFile, 
                 "disjunct reached, P=1/2^%u, Qs=%u:\n",
                 pl->rqs, qs );

        indent( s->indent * qs0, s->outFile );
        hard_printConjunction( c, hi );
    }

    return Hard_pathAdded;
}



static inline uint8_t countPaths( PathList * pl )
{
    uint8_t paths = 0;

    for ( ; pl != NULL; pl = pl->next, paths++ )
    {
    }
    
    return paths;
}



// Adds path. c is the conjunction. qs is the number of questions used
// to reach. qs0 is the length of qPath.
//   Path is added iff the number of random answer occurencies in
// questions path is <= o.
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_pathAdded (0) iff 
// path was added. Returns Hard_pathPostponed otherwise.
uint8_t conjHash_addIf( uint64_t c, uint8_t qs, HardInstance * hi,
                        uint8_t qs0, uint8_t o )
{
    Hard * h = hi->hard;
    Settings * s = hi->settings;

    // Get node.
    HashList * hl = conjHash_lookupHL( c, h );

    assert( c % h->godsN == 0 );

    if ( hl == NULL )
    {
        // This can't or shouldn't happen.
        fprintf( stderr, "\nError: conjunction not found in hash table "
                         "(%lu).\n\n", c );

        assert( false );

        return Hard_outOfMemory;
    }

    // Count random answer occurencies in questions path.
    uint8_t randomAnswersN = randomsAsked( c, h, qs0 );

    if ( randomAnswersN > o )
    {
        return Hard_pathPostponed;
    }


    // Update leeway and heuristic numbers.

    h->abortLeeway = max( h->abortLeeway - h->abortLeewayDecrement,
                          s->abortLeewayEnd );

    uint64_t powRs = (uint64_t)1 << randomAnswersN;

    if ( s->estimateHeuristic == 1 )
    {                          
        // We'll weigh the addition according to the probability,
        // 1/2^randomAnswersN.

        // We'll have to handle the case where h->subresultsFound is 0.
        if Unlikely( h->subresultsFound == 0 )
        {
            // What to do here? This will bias the estimate a bit,
            // when randomAnswersN != 0. But without some base,
            // what can you do? However, always, or almost always(?),
            // randomAnswers should be 0 here, so this should be fine.
            assert( h->subresSum == 0 );
            h->subresSum = qs;
        }
        else
        {
            h->subresSum += (double)qs / powRs  +  
                            ( ( h->subresSum / h->subresultsFound ) *
                            (double)(powRs-1) / powRs ); 
        }
        h->subresultsFound++;
    }
    else if ( s->estimateHeuristic == 2  ||  powRs == 1  || 
              common_randomNBiased(powRs) == 0 )
    {
        h->subresSum += qs;
        h->subresultsFound++;
    }


    // See if we want to abort the search.
    if ( h->subresultsFound > s->dontAbortUntil  &&
         (double)(h->subresSum) / h->subresultsFound >
         h->upperBound * h->abortLeeway )
    {
        // Print info.
        if ( s->verbosityVector & HardVerbosity_printAll )
        {
            indent( s->indent * qs0, s->outFile );
            fprintf( s->outFile, 
                     "Unpromising subsearch aborted. Estimated |Qs|: %f\n",
                     (double)(h->subresSum) / h->subresultsFound );
        }

        return Hard_aborted;
    }
    
    
    // Make new path node.
    PathList * pl = malloc( sizeof(PathList) );

    if ( pl == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return Hard_outOfMemory;
    }

    pl->next = hl->path;
    pl->qs = qs;
    pl->rqs = randomAnswersN;

    // Insert node first.
    hl->path = pl;

    // Print info.
    if ( s->verbosityVector & HardVerbosity_printAll )
    {
        indent( s->indent * qs0, s->outFile );
        fprintf( s->outFile, 
                 "disjunct reached, P=1/2^%u, Qs=%u:\n",
                 randomAnswersN, qs );

        indent( s->indent * qs0, s->outFile );
        hard_printConjunction( c, hi );
    }

    return Hard_pathAdded;
}



#if 0
// Old function using countPaths. Don't use. Use addIf above instead.
// Adds path. c is the conjunction. qs is the number of questions used
// to reach. qs0 is the length of qPath.
//   Path is added iff the number of paths to the conjunction is <= o.
//   Returns 2 iff not enough memory could be allocated. Return 1 iff
// path was added. Returns 0 otherwise.
static uint8_t conjHash_addIfPath( uint64_t c, uint8_t qs, Hard * h,
                                   uint8_t qs0, uint8_t o )
{
    // Get node.
    HashList * hl = conjHash_lookupHL( c, h );

    assert( c % h->godsN == 0 );

    if ( hl == NULL )
    {
        // This can't or shouldn't happen.
        fprintf( stderr, "\nError: conjunction not found in hash table "
                         "(%lu).\n\n", c );

        assert( false );

        return 2;
    }

    // Count paths.
    uint8_t paths = countPaths( hl->path );

    if ( paths > o )
    {
        return 0;
    }

    // Make new path node.
    PathList * pl = malloc( sizeof(PathList) );

    if ( pl == NULL )
    {
        fprintf( stderr, "\nError: not enough memory.\n\n" );

        return 2;
    }

    pl->next = hl->path;
    pl->qs = qs;
    pl->rqs = randomsAsked( c, h, qs0 );

    // Insert node first.
    hl->path = pl;

    return 1;
}
#endif



// Marks the current hash table state, so that the state can be recovered.
// Returns true iff there wasn't enough memory.
//   Adds marked node first in all paths.
bool conjHash_markState( Hard * h )
{
    ConjHash * ht = h->ht;

    uint64_t htSize = 1u << ht->logSize;

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * hl = (ht->tab)[k]; hl != NULL; hl = hl->next )
        {
            // Make new path node.
            PathList * pl = malloc( sizeof(PathList) );

            if ( pl == NULL )
            {
                fprintf( stderr, "\nError: not enough memory.\n\n" );

                return true;
            }

            pl->next = hl->path;
            pl->qs = RQsUndef;
            pl->rqs = RQsUndef;

            // Insert node first.
            hl->path = pl;
        }
    }

    return false;
}


// Marks the current hash table state, so that the state can be recovered.
// Returns true iff there wasn't enough memory.
//   Only local conjunctions, from start to end, will be marked. 
// Actually, the whole bucket of local conjunctions will be marked. Not!
// We'll mark only paths, and if there is only one element in a bucket
// we won't check the conjunction but just assume that it's the sought one.
// There might be problems later on with e.g. undo2 if we mark whole buckets.
// Because, with whole buckets marked, paths can be marked more than once,
// if two or more local conjunctions are in the same bucket, which might
// spell trouble for e.g. undo2Loc.
bool conjHash_markStateLocally( Hard * h, uint64_t start, uint64_t end )
{
    ConjHash * ht = h->ht;

    uint8_t n = h->godsN;

    for ( uint64_t i = start; i != end; i += n )
    {
        HashList * hl = (ht->tab)[hashFNV( i, h )];

        // Get the i node. This will be fast.
        for ( ; hl->next != NULL  &&  !equalConj( i, h, hl->conj );
                hl = hl->next )
        {
        }

        assert3( equalConj( i, h, hl->conj ) );

        // Make new path node.
        PathList * pl = malloc( sizeof(PathList) );

        if ( pl == NULL )
        {
            fprintf( stderr, "\nError: not enough memory.\n\n" );

            return true;
        }

        pl->next = hl->path;
        pl->qs = RQsUndef;
        pl->rqs = RQsUndef;

        // Insert node first.
        hl->path = pl;
    }

    return false;
}



// Recovers the last marked state.
//   Deletes all nodes up to but not including the first marked node
// in all paths.
void conjHash_undo( Hard * h )
{
    ConjHash * ht = h->ht;

    uint64_t htSize = 1u << ht->logSize;

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * hl = (ht->tab)[k]; hl != NULL; hl = hl->next )
        {
            PathList * pl = hl->path;

            while ( pl->rqs != RQsUndef )
            {
                PathList * prevPL = pl;
                pl = pl->next;
                free(prevPL);
            }

            hl->path = pl;
        }
    }

    return;
}


// Deletes all nodes from but not including the first mark up to and
// including the second marked node, in all paths.
void conjHash_undo2( Hard * h )
{
    ConjHash * ht = h->ht;

    uint64_t htSize = 1u << ht->logSize;

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * hl = (ht->tab)[k]; hl != NULL; hl = hl->next )
        {
            PathList * pl = hl->path;

            // Get first mark.
            while ( pl->rqs != RQsUndef )
            {
                pl = pl->next;
            }

            // Delete segment.

            PathList * pl2 = pl->next;

            while ( pl2->rqs != RQsUndef )
            {
                PathList * prevPL = pl2;
                pl2 = pl2->next;
                free(prevPL);
            }

            // Delete second mark.
            pl->next = pl2->next;
            free(pl2);
        }
    }

    return;
}


// Recovers the last locally marked state.
//   Deletes all nodes up to but not including the first marked node
// for local conjunctions.
//   Only local conjunctions, from start to end, will be undone. 
void conjHash_undoLocally( Hard * h, uint64_t start, uint64_t end )
{
    ConjHash * ht = h->ht;

    uint8_t n = h->godsN;

    for ( uint64_t i = start; i != end; i += n )
    {
        HashList * hl = (ht->tab)[hashFNV( i, h )];

        // Get the i node. This will be fast.
        for ( ; hl->next != NULL  &&  !equalConj( i, h, hl->conj );
                hl = hl->next )
        {
        }

        assert3( equalConj( i, h, hl->conj ) );

        PathList * pl = hl->path;

        while ( pl->rqs != RQsUndef )
        {
            PathList * prevPL = pl;
            pl = pl->next;
            free(prevPL);
        }

        hl->path = pl;
    }

    return;
}


// Deletes all nodes from but not including the first mark up to and
// including the second marked node, for local conjunctions.
//   Only local conjunctions, from start to end, will be undone. 
void conjHash_undo2Locally( Hard * h, uint64_t start, uint64_t end )
{
    ConjHash * ht = h->ht;

    uint8_t n = h->godsN;

    for ( uint64_t i = start; i != end; i += n )
    {
        HashList * hl = (ht->tab)[hashFNV( i, h )];

        // Get the i node. This will be fast.
        for ( ; hl->next != NULL  &&  !equalConj( i, h, hl->conj );
                hl = hl->next )
        {
        }

        assert3( equalConj( i, h, hl->conj ) );

        PathList * pl = hl->path;

        // Get first mark.
        while ( pl->rqs != RQsUndef )
        {
            pl = pl->next;
        }

        // Delete segment.

        PathList * pl2 = pl->next;

        while ( pl2->rqs != RQsUndef )
        {
            PathList * prevPL = pl2;
            pl2 = pl2->next;
            free(prevPL);
        }

        // Delete second mark.
        pl->next = pl2->next;
        free(pl2);
    }

    return;
}



// Removes the last mark.
void conjHash_removeLastMark( Hard * h )
{
    ConjHash * ht = h->ht;

    uint64_t htSize = 1u << ht->logSize;

    for ( uint64_t k = 0; k != htSize; k++ )
    {
        for ( HashList * hl = (ht->tab)[k]; hl != NULL; hl = hl->next )
        {
            PathList * pl = hl->path;

            if ( pl->rqs == RQsUndef )
            {
                // Handle case where mark is first.
                hl->path = pl->next;
                free(pl);
            }
            else
            {
                PathList * prevPL = pl;
                pl = pl->next;

                while ( pl->rqs != RQsUndef )
                {
                    prevPL = pl;
                    pl = pl->next;
                }

                prevPL->next = pl->next;
                free(pl);
            }
        }
    }

    return;
}


// Removes the last local mark.
//   Only local conjunctions, from start to end, will be undone. 
void conjHash_removeLastLocalMark( Hard * h, uint64_t start, uint64_t end )
{
    ConjHash * ht = h->ht;

    uint8_t n = h->godsN;

    for ( uint64_t i = start; i != end; i += n )
    {
        HashList * hl = (ht->tab)[hashFNV( i, h )];

        // Get the i node. This will be fast.
        for ( ; hl->next != NULL  &&  !equalConj( i, h, hl->conj );
                hl = hl->next )
        {
        }

        assert3( equalConj( i, h, hl->conj ) );

        PathList * pl = hl->path;

        if ( pl->rqs == RQsUndef )
        {
            // Handle case where mark is first.
            hl->path = pl->next;
            free(pl);
        }
        else
        {
            PathList * prevPL = pl;
            pl = pl->next;

            while ( pl->rqs != RQsUndef )
            {
                prevPL = pl;
                pl = pl->next;
            }

            prevPL->next = pl->next;
            free(pl);
        }
    }

    return;
}

