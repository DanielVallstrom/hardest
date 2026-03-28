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


#ifndef readBounds_H
#define readBounds_H


#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "hard.h"


// Parses the bounds file. Returns true iff an error occurred.
bool readBounds_readFile( HardInstance * hi );

// Writes bound to the upper bounds file. Returns true iff an error occurred.
//   seed and boundUsed should be the seed and bound used for the search that 
// found the new bound.
//   Also updates hi.
//   Rows will end with CRLF, following some csv definition or convention,
// apparently. 
//   Uses tmpnam, which is deprecated. However, there is no standard C
// alternative.
bool readBounds_write( HardInstance * hi, double bound, uint64_t seed,
                       double boundUsed );

// Increments the replication counter in the upper bounds file. Returns true 
// iff an error occurred.
//   seed and boundUsed should be the seed and bound used for the search that 
// found the new bound.
//   Also replaces the replication command, if s->noteRep > 1.
//   Rows will end with CRLF, following some csv definition or convention,
// apparently. 
//   Uses tmpnam, which is deprecated. However, there is no standard C
// alternative.
bool readBounds_noteRep( HardInstance * hi, uint64_t seed, double boundUsed );

#endif  // readBounds_H
