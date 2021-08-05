/*
 *    Name: PTest001
 * Purpose: Performance Testing for ZVector Library
 *  Author: Paolo Fabio Zaino
 *  Domain: General
 * License: Copyright by Paolo Fabio Zaino, all rights reserved
 *          Distributed under MIT license
 */

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ccpal.h"

#if (  defined(_MSC_VER) )
 // Silly stuff that needs to be added for Microsoft compilers
 // which are still at the MS-DOS age apparently...
#define ZVECTORH "../src/zvector.h"
#else
#define ZVECTORH "zvector.h"
#endif
#include ZVECTORH

#define MAX_ITEMS 10000000

// Setup tests:
char *testGrp = "001";
uint8_t testID = 1;

#if ( OS_TYPE == 1 )

void populate_vector(vector v)
{
    /* 
	   This macro initialise the perf mesurement library.
       have a look at my CCPal project on github for more details.
	*/
    CCPAL_INIT_LIB;

    printf("Test %s_%d: Insert %d elements (one by one) at the end of the vector and check how long this takes:\n", testGrp, testID, MAX_ITEMS);
    int i = 0;
	CCPAL_START_MEASURING;

    while ( i++ < MAX_ITEMS )
        vect_add(v, &i);
    
    CCPAL_STOP_MEASURING;

    // Returns perf analysis results:
    CCPAL_REPORT_ANALYSIS

    printf("done.\n");
    testID++;

    fflush(stdout);
}

int main()
{
    /* 
	   This macro initialise the perf mesurement library.
       have a look at my CCPal project on github for more details.
	*/
	CCPAL_INIT_LIB;

    printf("=== PTest%s ===\n", testGrp);
    printf("Testing basic vector PERFORMANCE\n");

    fflush(stdout);

    printf("Test %s_%d: Create a vector of 10 elements and using int for the vector data:\n", testGrp, testID);
    vector v;
    v = vect_create(10, sizeof(int), ZV_BYREF);
    printf("done.\n");
    testID++;

    fflush(stdout);

    // Populate the vector and mesure how long it takes:
    populate_vector(v);

    printf("Test %s_%d: check if the size of the vector is now %d:\n", testGrp, testID, MAX_ITEMS);
    assert(vect_size(v) == MAX_ITEMS);
    printf("done.\n");
    testID++;

    fflush(stdout);

    /*
    printf("Test %s_%d: Add elements in the middle of the vector:\n", testGrp, testID);
    i=555555;
    vect_add_at(v, &i, 100);
    assert(*((int *)vect_get_at(v, 100)) == i);
    assert(*((int *)vect_get_at(v, 101)) == 100);
    printf("done.\n");
    testID++;
    */

    printf("Test %s_%d: Remove vector elements one by one (from the end of the vector) and test how long it takes:\n", testGrp, testID);

	CCPAL_START_MEASURING;

    while ( !vect_is_empty(v) )
        vect_delete(v);
    
    CCPAL_STOP_MEASURING;

    // Returns perf analysis results:
    CCPAL_REPORT_ANALYSIS

    printf("done.\n");
    testID++;

    fflush(stdout);

    printf("Test %s_%d: check if vector is empty:\n", testGrp, testID);
    assert(vect_is_empty(v));
    printf("done.\n");
    testID++;

    fflush(stdout);

    printf("Test %s_%d: Check if vector size is now 0 (zero):\n", testGrp, testID);
    assert(vect_size(v) == 0);
    printf("done.\n");
    testID++;

    fflush(stdout);

    // Re-populate the vector again and measure how long it takes:
    populate_vector(v);


    printf("Test %s_%d: destroy the vector:\n", testGrp, testID);
    vect_destroy(v);
    printf("done.\n");
    testID++;

    fflush(stdout);

    printf("================\n\n");

    return 0;
}

#else
int main()
{
    printf("=== PTest%s ===\n", testGrp);
    printf("Testing ZVector Library PERFORMANCE:\n");

    printf("Skipping test because this OS is not yet supported for perf tests, sorry!\n");

    printf("================\n\n");

    return 0;
}
#endif