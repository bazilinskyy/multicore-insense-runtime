/*
 * Setting affinity to POSIX threads.
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

#include "affinity.h"
#include "GlobalVars.h"

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

#ifndef NULL
#define NULL   ((void *) 0)
#endif

int lastUsedCore = -1; // Note down ID of the core that was used for setting affinity last time.

/*
 * Retrieve a number of cores available on the machine
 * Platform and system independent.
 */
int receiveNumberCores() {
	return sysconf(_SC_NPROCESSORS_ONLN);
}

/*
 * Set affinity of POSIX thred with pthread_t thread.
 * AFFINITY_ALGO is defined in GlobalVars.h. Refer to that file for possible values.
 */
int setAffinity(pthread_t thread) {
#if AFFINITY_ALGO == 1
	// Round-robin
	return setAffinityToCore(thread, receiveCoreIdForThread(thread));
#elif AFFINITY_ALGO == 2
	// Set all threads to one single core
	return setAffinityToCore(thread, 0);
#else
	// Allow OS take care of setting affinity
	return 1;
	// Set affinity to a random core
	//return setAffinityToCore(thread, rand() % receiveNumberCores());
#endif
}

/*
 * Set affinity of POSIX thread "thread" to core with ID defined by "core".
 *
 * Current version is compatible only with Linux. Mac OS requires a different set of tools for setting affinity.
 */
int setAffinityToCore(pthread_t thread, int core) {
	// #if TARGET_OS == linux
	// 	PRINTFMC("Core ID: %d\n", core);
	// 	if (core >= receiveNumberCores())
	// 		return NULL;
	// 	cpu_set_t cpuset;
	// 	CPU_ZERO(&cpuset);
	// 	CPU_SET(core, &cpuset);

	// 	int s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	// 	if (s != 0)
	//                handle_error_en(s, "pthread_setaffinity_np");

	// 	return s;
	// #endif
	// #if TARGET_OS == macos
	// 	return NULL;
	// #endif

	if (core >= receiveNumberCores()) // First check if value of core exceeds a number of cores in the processor
		return -1;
	// Define CPUSET which describes the CPU
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	// Set affinity
	int s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0) // Check for errors
		handle_error_en(s, "pthread_setaffinity_np");

	return s;
}

/*
 * Check affinity of a POSIX thread with pthread_t ID thread
 */
cpu_set_t getAffinityThread(pthread_t thread) {
	int s, j;
	cpu_set_t cpuset;
	s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "pthread_getaffinity_np");

	PRINTFMC("Location of thread with ID %u is ", thread);
	for (j = 0; j < CPU_SETSIZE; j++)
		if (CPU_ISSET(j, &cpuset))
			PRINTFMC("CPU %d\n", j);
	return cpuset;
}

/*
 * Receive next core to be used for affinity.
 * Deprecated in the current implementation.
 */
int receiveCoreIdForThread(pthread_t thread) {
#if AFFINITY_ALGO == 1
	int newCore = ++lastUsedCore;
	if (newCore >= receiveNumberCores()) {
		lastUsedCore = 0;
		newCore = 0;
	}
	return newCore;
#else
	return -1;
#endif
}
