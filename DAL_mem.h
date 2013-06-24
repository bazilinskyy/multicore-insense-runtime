/*
 * Memory management function declarations
 *
 *
 * Copyright (c) 2013, Jonathan Lewis, Alan Dearle, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
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

#ifndef DAL_MEM_H_
#define DAL_MEM_H_

#include <stdlib.h>
#include <stdio.h>
#include "Bool.h"
#include "GlobalVars.h"
#include <pthread.h>
#include "time.h"
#include "ThreadSafeList.h"
#include "DAL_error.h"
#include <string.h>
#include "strmap.h"
#include <sys/mman.h>



extern pthread_t mainThread; // ID of the main thread.

// Choose which memory allcator to use (Big heap / small heaps)
#if HEAPS
#define BASE_mem_alloc SH_alloc
#define BASE_mem_free SH_free
#else
#define BASE_mem_alloc BH_alloc
#define BASE_mem_free BH_free
#endif

typedef void (*decRefFunc_t)(void *pntr);

typedef struct MemHeader {
	unsigned long ref_count :63; // 64-bit architecture
	unsigned short mem_contains_pointers :1;
}*MemHeader, MemHeaderStruct;

/*
 * Structure used for passing pthread_t and memory address.
 * Hint taken from: http://stackoverflow.com/questions/8976419/passing-more-than-one-parameter-to-pthread-create
 */
struct shMapType {
	pthread_t thread_id;		// ID of the thread that is linked to the small heap
	unsigned int * memArea;		// The actual memory range used for allocating memory within the small heap
	unsigned int memAreaSize;	// Size of the small heap
	unsigned int available;		// How much memory is available for allocation
};

// Every object that contains pointers has a pntr to a function decRef that 
// decrements the references for memory refernced by its pointer fields
typedef struct PointerContainer {
	decRefFunc_t decRef;
} PointerContainerStruct, *PointerContainerPNTR;

// CopyableObject logically extends PointerContainer
// Every object that can be copied has a reference to a copy function
typedef struct CopyableObject {
	decRefFunc_t decRef;
	void *(*copyObject)(void *this, bool);
} CopyableObjectStruct, *CopyableObjectPNTR;

extern void DAL_assign(void *generic_var_pntr, void *new_mem);
extern void* DAL_alloc(size_t size, bool contains_pointers);
extern void* DAL_SH_alloc(size_t size, bool contains_pointers, pthread_t * t);
extern unsigned DAL_getRef(void *pntr);
extern bool DAL_mem_contains_pointers(void *pntr);
extern void DAL_mem_set_contains_pointers(void *pntr, bool mem_contains_pointers);
extern void DAL_decRef(void *pntr);
extern void DAL_incRef(void *pntr);
extern void DAL_modRef_by_n(void *pntr, int n);
extern void DAL_free(void *pntr);

// Used with small heaps
extern void* SH_create_small_heap();
extern void* SH_alloc(unsigned int size);
extern void* SH_alloc_at_base(unsigned int size, struct shMapType * shMapEntry);
extern void SH_free(void *pntr);
struct shMapType * SH_init(void *ptr, pthread_t thread);
void * log_into_file(char * string, void * ptr, unsigned int size);

// Used with the big heap
extern void* BH_alloc(unsigned int size);
extern void BH_free(void *pntr);

#endif /*DAL_MEM_H_*/
