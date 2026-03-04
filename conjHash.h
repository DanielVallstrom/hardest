// Module for conjunction hashing.


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


#ifndef conjHash_H
#define conjHash_H


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Forward declarations:
typedef struct ConjHash_ ConjHash;

#include "hard.h"


// Indicates a marking node.
#define RQsUndef (UINT8_MAX)

// HT value indicating error (not enough memory).
//#define HT_error NULL
// UINT64_MAX

// Default max load before the table is extended. A doubling is done when
// maxLoad < nrOfEntries/2^logSize.
#define DefaultMaxLoad (0.75)


// The hash table value type. A list of ways to reach a conjunction.
struct PathList_
{
    uint8_t qs;   // The number of questions asked.
    uint8_t rqs;  // How many randoms that were questioned on the path
                  // here. To calculate the probability.
                  //   The average number of questions used for a
                  // disjunct will be 1/2^rqs1 * qs1 + ... + 1/2^rqsk * qsk.
                  //   rqs == RQsUndef indicates a marking node. These are
                  // used to save states so that sub-searches can be undone.
                  //   When traversing paths, we'll just skip the marking
                  // nodes. (Although, if left in, marked nodes will not
                  // affect things since the probability weight for them is
                  // 1/2^256.)
    struct PathList_ * next;
};

typedef struct PathList_ PathList;


// The value type.
typedef PathList * HT;


// The list type used for hash table entries.
struct HashList_
{
    God * conj;  // A conjunction. What's hashed.
    PathList * path;  // The ways the conjunction is detected.
    struct HashList_ * next;
};

typedef struct HashList_ HashList;


// The structure used to store a hash table.
struct ConjHash_
{
    // The hash table.
    HashList * * tab;

    // Number of entries.
    uint64_t nrOfEntries;

    // The log2 size of the hash table.
    uint8_t logSize;

    // Max load before the table is extended. A doubling is done when
    // maxLoad < nrOfEntries/2^logSize.
    double maxLoad;

    // Number of collisions.
    uint64_t nrOfCollisions;

    //uint64_t nrOfGods;  // The number of conjuncts.
};

typedef struct ConjHash_ ConjHash;



// Deletes a hash-table.
void conjHash_delete( ConjHash * h );

// Doubles the hash-table size. Returns true iff the request couldn't be
// satisfied.
//   If the size is already at max, maxLoad will be increased and false
// returned.
bool conjHash_doubleSize( ConjHash * h );

// Returns the value of conjunction c. c should be an index pointing 
// into h->gods.
//   Returns NULL if no entry was found.
PathList * conjHash_lookup( uint64_t c, Hard * h );

// Returns the HashList of conjunction c. c should be an index pointing 
// into h->gods.
//   Returns NULL if no entry was found.
HashList * conjHash_lookupHL( uint64_t c, Hard * h );


// vvv   Extends the table if the load becomes larger than maxLoad.
//   HT_error is return if the table couldn't be extended (because of not
// enough memory) or the entry couldn't be added.


// Creates a new hash-table of size 2^logSize. Returns NULL iff the
// request couldn't be satisfied. The table entries are all initialized to
// NULL.
//   Any small value, e.g. 6, for logSize is fine if you don't know how big
// table you need.
//   I don't know what the best value for maxLoad is --- I guess it depends
// to some extent on the circumstances. Perhaps a value of about 0.8 or 0.9
// is good??
//   cLen is the length of a conjunction, in bytes.
ConjHash * conjHash_new( Hard * h );
//unsigned int logSize, double maxLoad, uint8_t cLen );

// Initializes the hash table. We'll put all conjunctions in it.
//   Returns true iff not enough memory could be allocated.
//   poss0R0 is the number of possibilities with starting R. (We should
// just get rid of poss0R0 altogether, everywhere. And use the fnd1 approach.)
bool conjHash_initHash( Hard * h, uint64_t poss0R0 );

// Prints the hash table to file in order.
void conjHash_print( Hard * h, FILE * f );

// Collects and prints some statistics about the hash table to f.
void conjHash_stats( Hard * h, FILE * f );

// Returns the hash table array pointer for a conjunction.
HashList * * conjHash_bucket( uint64_t c, Hard * h );

// Adds path. c is the conjunction. qs is the number of questions used
// to reach. qs0 is the length of qPath.
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_pathAdded (0)
// otherwise.
uint8_t conjHash_add( uint64_t c, uint8_t qs, HardInstance * hi, uint8_t qs0 );

// Adds path. c is the conjunction. qs is the number of questions used
// to reach. qs0 is the length of qPath.
//   Path is added iff the number of paths to the conjunction is <= o.
//   Returns Hard_outOfMemory iff there wasn't enough memory. Returns
// Hard_aborted iff the search was aborted. Returns Hard_pathAdded (0) iff 
// path was added. Returns Hard_pathPostponed otherwise.
uint8_t conjHash_addIf( uint64_t c, uint8_t qs, HardInstance * hi, 
                        uint8_t qs0, uint8_t o );

// Calculates the average number of questions asked.
double conjHash_questionAvg( Hard * h );

// Marks the current hash table state, so that the state can be recovered.
// Returns true iff there wasn't enough memory.
bool conjHash_markState( Hard * h );

// Recovers the last marked state.
//   Deletes all nodes up to but not including the first marked node
// in all paths.
void conjHash_undo( Hard * h );

// Deletes all nodes from but not including the first mark up to and
// including the second marked node, in all paths.
void conjHash_undo2( Hard * h );

// Removes the last mark.
void conjHash_removeLastMark( Hard * h );

#endif // conjHash_H

