/*
 *    Name: ITest001
 * Purpose: Integration Testing ZVector Library
 *  Author: Paolo Fabio Zaino
 *  Domain: General
 * License: Copyright by Paolo Fabio Zaino, all rights reserved
 *          Distributed under MIT license
 */

#if ( __GNUC__ <  6 )
#define _BSD_SOURCE
#endif
#if ( __GNUC__ >  5 )
#define _DEFAULT_SOURCE
#endif

#define UNUSED(x)			(void)x

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if ( ZVECT_COMPTYPE == 1 )
#include <unistd.h>
#elif ( ZVECT_COMPTYPE == 2 )
#include <windows.h>
#include <process.h>
#include <io.h>
#endif
#include <assert.h>
#include <string.h>

#if ( defined(_MSC_VER) )
 // Silly stuff that needs to be added for Microsoft compilers
 // which are still at the MS-DOS age apparently...
#define ZVECTORH "../src/zvector.h"
#else
#define ZVECTORH "zvector.h"
#endif
#include ZVECTORH

#define MAX_ITEMS 200

// Setup tests:
char *testGrp = "001";
uint8_t testID = 1;

#if ( ZVECT_THREAD_SAFE == 1 ) && ( OS_TYPE == 1 )

#include<pthread.h>

pthread_t tid[2]; // Two threads IDs

void increment_elements(void *element) {
	// Let's convert element into the right
	// C type, however we use a pointer to int
	// so that we can modify element content
	// directly without the need to copy it and
	// return it.
	int *number = (int *)element;
	*number += 1;
}

void multiply_elements(void *element) {
	// Let's convert element into the right
	// C type, however we use a pointer to int
	// so that we can modify element content
	// directly without the need to copy it and
	// return it.
	int *number = (int *)element;
	*number *= 5;
}

void add_a_int(vector v) {
	int myint = 100;
	vect_add(v, &myint);
}

void * doSomething1(void *arg)
{
	vector *v = (vector *)arg;

#ifdef ZVECT_SFMD_EXTENSIONS
	// We have SFMD extensions enabled so let's use them for this test!
	printf("Test %s_%d:  -  Apply function 'increment_elements' to the entire vector at once:\n",
		testGrp, testID);
	fflush(stdout);

		// Let's run a vector wide function:
		vect_apply((vector)v, increment_elements);

		printf("Test %s_%d:  -  All items incremented.\n", testGrp, testID);
#endif // ZVECT_SFMD_EXTENSIONS
#ifndef ZVECT_SFMD_EXTENSIONS
	// We DO NOT have SFMD extensions enabled so let's use a regular loop!
	printf("Test %s_%d:  -  Incrementing each vector's item (one-by-one):\n", testGrp, testID);
	fflush(stdout);

		int i;
		for (i = 0; i < MAX_ITEMS; i++) {
			// Let's increment each vector element one by one:
			increment_elements(vect_get_at((vector)v, i));
		}

		printf("Test %s_%d:  -  All items incremented.\n", testGrp, testID);
#endif  // ZVECT_SFMD_EXTENSIONS

	pthread_exit(NULL);
	return NULL;
}

void * doSomething2(void *arg) {
	vector *v = (vector *)arg;

#ifdef ZVECT_SFMD_EXTENSIONS
	// We have SFMD extensions enabled so let's use them for this test!

	printf("Test %s_%d:  -  Apply function 'multiply_elements' to the entire vector at once:\n",
		testGrp, testID);
	fflush(stdout);

		// Let's run a vector wide function:
		vect_apply((vector)v, multiply_elements);

		printf("Test %s_%d:  -  All items multiplied.\n", testGrp, testID);
#endif // ZVECT_SFMD_EXTENSIONS
#ifndef ZVECT_SFMD_EXTENSIONS
	// We DO NOT have SFMD extensions enabled so let's use a regular loop!
	printf("Test %s_%d:  -  Multiplying each vector's item (one-by-one):\n", testGrp, testID);
	fflush(stdout);

		int i;
		for (i = 0; i < MAX_ITEMS; i++) {
			// Let's increment each vector element one by one:
			multiply_elements(vect_get_at((vector)v, i));
		}

		printf("Test %s_%d:  -  All items multiplied.\n", testGrp, testID);
#endif

	pthread_exit(NULL);
	return NULL;
}

int main()
{
	printf("=== ITest%s ===\n", testGrp);
	printf("Testing Thread_safe features:\n");

	printf("Test %s_%d: Create a vector of 2 elements and using int for the vector data:\n", testGrp, testID);
	fflush(stdout);

		vector v;
		v = vect_create(2, sizeof(int), ZV_SEC_WIPE);

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Insert %d elements and check if they are stored correctly:\n",
		testGrp, testID, MAX_ITEMS);
	fflush(stdout);

		int i = 0;
		for (i = 0; i < MAX_ITEMS; i++) {
			// Let's add a new value in the vector:
			vect_add(v, &i);

			// Let's check if the vector size has grown correctly:
			assert(vect_size(v) == (zvect_index)i + 1);

			// Let's retrieve the value from the vector correctly:
			// For beginners: this is how in C we convert back a void * into the original dtata_type
			int value = *((int *)vect_get_at(v, i));

			// Let's test if the value we have retrieved is correct:
			assert(value == i);
		}

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Spin 2 threads and use them to manipoulate the vector above.\n", testGrp, testID);
	fflush(stdout);

		int err = 0;
		i = 0;
		err = pthread_create(&(tid[i]), NULL, &doSomething1, v);
		if (err != 0)
			printf("Can't create thread :[%s]\n", strerror(err));
		i++;

		// Let's start the threads:
		pthread_join(tid[0], NULL);

		err = pthread_create(&(tid[i]), NULL, &doSomething2, v);
		if (err != 0)
			printf("Can't create thread :[%s]\n", strerror(err));
		i++;

		pthread_join(tid[1], NULL);

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Check vector size:\n", testGrp, testID);
	fflush(stdout);

	printf("Size now: %d\n", vect_size(v));

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Check the vector to see if elements value is coerent:\n", testGrp, testID);
	fflush(stdout);

		for (i = 0; i < MAX_ITEMS; i++) {
			printf ("Checking item: %*d = ", 4, i);

			// Let's retrieve the value from the vector correctly:
			// For beginners: this is how in C we convert back a void * into the original dtata_type
			int value = *((int *)vect_get_at(v, i));

			printf("%*d\n", 4, value);
			fflush(stdout);

			// Let's test if the value we have retrieved is correct:
			assert(value == (( i + 1 ) * 5) );
		}

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Clear vector:\n", testGrp, testID);
	fflush(stdout);

		vect_clear(v);

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: Check if vector size is now 0 (zero):\n", testGrp, testID);
	fflush(stdout);

		assert(vect_size(v) == 0);

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("Test %s_%d: destroy the vector:\n", testGrp, testID);
	fflush(stdout);

		vect_destroy(v);

	printf("done.\n");
	testID++;

	fflush(stdout);

	printf("================\n");

	return 0;
}

#else

int main()
{
	printf("=== ITest%s ===\n", testGrp);
	printf("Testing Thread_safe features:\n");

	printf("Skipping test because library has been built without THREAD_SAFE enabled or on a platform that does not supports pthread.\n");

	printf("================\n\n");

	return 0;
}

#endif  // THREAD_SAFE
