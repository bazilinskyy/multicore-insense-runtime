/*
 * Memory management functions used with the big heap memory allocation scheme.
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


#include "DAL_mem.h"

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

pthread_t mainThread;			// ID of the main thread.
pthread_mutex_t alloc_lock;		// Mutex used to protect allocation and freeing of memory.

#if DEBUG 
static void *last_max_malloc;
#endif

#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define CHAN_PRINT(...) channel_print(__VA_ARGS__)
#else
#define PRINTF(...)
#define CHAN_PRINT(...)
#endif

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

/*
 * Allocator of memory, allocates into a big heap. Uses malloc().
 * Allocation is protected by a mutex. Uncoment code to enable extra mutes protection.
 */
void * BH_alloc(unsigned size) {
	// Lock mutex
	// pthread_mutex_lock(&alloc_lock);
	// Allocate memory
	void * result = malloc(size);
	// Unlock mutex
	// pthread_mutex_unlock(&alloc_lock);
	// Return
	return result;
}

/*
 * Freeing memory from the big heap. Uses free().
 * Freeing is not protected by a mutex. Uncoment code to enable extra mutes protection.
 */
void BH_free(void *ptr) {
	// Lock mutex
	// pthread_mutex_lock(&alloc_lock);
	// Free memory
	free(ptr);
	// Unlock mutex
	// pthread_mutex_unlock(&alloc_lock);
}
