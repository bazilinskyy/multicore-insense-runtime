/*
 * Main file for the runtime.
 *
 *
 * Copyright (c) 2013, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
 * School of Computer Science, St. Andrews University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "GlobalVars.h"
#include "ThreadSafeList.h"
#include "component.h"
#include "DAL_mem.h"
#include "pthread.h"
#include "cache_line_size.h"
#include "time.h"

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

#if MCDEBUG
#include <stdio.h>
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

pthread_mutex_t thread_lock;
pthread_t mainThread;
struct threadSafeList* threadList;
struct threadSafeList* SHList;
struct threadSafeList* mallocList;

/*
 * Entry point of Multi-core Insense runtime.
 */
int main() {
	PRINTFMC("Cache line size: %dB\n", cache_line_size());PRINTFMC("Main thread: %u\n", (unsigned) pthread_self());

#if HEAPS // Small heaps
	// Initialize mutex
	if (pthread_mutex_init(&thread_lock, NULL ) != 0) {
		PRINTF("Mutex initialization failed.\n");
		return NULL;
	}
#else // Big heap
	// Initialize mutex
	if (pthread_mutex_init(&alloc_lock, NULL ) != 0) {
		PRINTF("Mutex initialization failed.\n");
		return NULL ;
	}
#endif

	mainThread = pthread_self(); // Note the ID of the main thread.

	// Create a list for storing references to p-threads
	threadList = listCreate();

	// Create map used to store memory locations of small heaps (using Thread safe list)
	SHList = listCreate();

	// Create map used to store memory locations what is allocated using malloc
	mallocList = listCreate();

// Start recording execution time
#if TIMING
	// CPU time
	struct timespec start, finish;
	double elapsed;
	//clock_gettime(CLOCK_MONOTONIC, &start);
	// User time
	time_t start_t, end_t;
	double diff_t;
	time(&start_t);
#endif

	// Call primordial_main.
	primordial_main(NULL );

	// Join all p-threads
	if (threadList != NULL ) {
		listJoinThreads(threadList);
	}

// Stop recording execution time
#if TIMING
	// CPU time
	//clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	printf("CPU:  %f seconds elapsed\n", elapsed);
#endif

	// Destroy lists and free memory
	listDestroy(threadList);
	listDestroy(SHList);
	listDestroy(mallocList);
	pthread_mutex_destroy(&thread_lock); 	// Destroy mutex lock used with pthreads
	pthread_mutex_destroy(&alloc_lock); 	// Destroy mutex lock used with alloc and free in the big heap scheme

	return 1;
}
