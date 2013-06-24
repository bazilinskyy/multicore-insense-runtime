/*
 * Defining global variables used in runtime and compiled Insense programs.
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


#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

#include "Bool.h"
#include "ThreadSafeList.h"
#include "pthread.h"

bool success;
extern pthread_mutex_t thread_lock; // Mutex used to avoid problems with scheduling that comes when small heaps are utilised.
extern pthread_mutex_t alloc_lock; 	// Mutex used with alloc and free with the big heap.

#define BEHAVIOUR_COUNT -1 // Number of times behaviour of components is run. -1 for unlimited number of runs. 

#define DEBUG 0			// Output debugging information, defined for InceOS. Possible values: 0, 1, 2.
#define MCDEBUG 0		// Output debugging information, defined for Multi-core Insense. Possible values: 0, 1.
#define HEAPS 0			// 1 - use small heaps; 2 - use big heap.
#define AFFINITY_ALGO 0 // Algorithm for setting affinity: 0 - get Core ID on random, dynamically; 1 - Round Robin; 2 - All components are on core with ID = 0
#define TIMING 0		// Measure and output execution time.

#endif
