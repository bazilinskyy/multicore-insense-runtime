/**
 *
 * Start of channel implementation for UNIX
 *
 * Send, receive, bind, unbind are implemented according to SPIN paper algorithms
 * Select operation is missing
 *
 * Absolutely no guarantees with this code, nevcer been tested by me
 *
 * @author jonl, based on code from Andrew Bell
 *
 * 26.04.2013
 *
 */

#include "channel.h"

static void Channel_decRef(Channel_PNTR pntr);
static char *file_name = "Channel";

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

Channel_PNTR channel_create(chan_dir direction, int typesize, bool contains_pointers) {
	PRINTFMC("Create channel\n");
	Channel_PNTR this = (Channel_PNTR)DAL_alloc(sizeof(struct Channel), true);
	if(this == (void*) 0){
		DAL_error(CHAN_OUT_OF_MEMORY_ERROR);
		return NULL;
	}

	this->decRef = Channel_decRef;
	this->direction = direction;
	this->typesize = typesize;
	this->ready = false;
	this->nd_received = false;
	DAL_assign(&(this->connections), Construct_List());	// empty list of connections

	// Initialise mutexes and semaphores
	sem_init(&(this->conns_sem), 0, 0);		
	pthread_mutex_init(&(this->mutex), NULL);
	sem_init(&(this->blocked), 0, 0);
	sem_init(&(this->actually_received), 0, 0);

	return(this);
}


void Channel_decRef(Channel_PNTR this){
	channel_unbind(this);                   // disconnect from all other chans
	DAL_decRef(this->connections);          // GC connections list
	sem_destroy(&(this->conns_sem));		// now destroy mutexes and semaphores
	pthread_mutex_destroy(&(this->mutex));
	sem_destroy(&(this->blocked));
	sem_destroy(&(this->actually_received));
}


bool channel_bind(Channel_PNTR id1, Channel_PNTR id2) {
	PRINTFMC("Bind channels ID1: %d and ID2: %d\n", id1, id2);
	pthread_mutex_lock(&conn_op_mutex);
	
	// check not both CHAN_IN or CHAN_OUT
	if(id1->direction == id2->direction) {
		PRINTFMC("Bind directions are the same\n");
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	} 
	if (id1->typesize != id2->typesize) {
		PRINTFMC("Bind typesizes are different\n");
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	}

	pthread_mutex_lock(id1->direction == CHAN_IN ? &(id1->mutex) : &(id2->mutex));
	pthread_mutex_lock(id1->direction == CHAN_IN ? &(id2->mutex) : &(id1->mutex));

	// check not already connected
	// assuming bind always adds to both channels' lists, we only need to check one channel for the other
	if(containsElement(id1->connections, (void*)id2)) {
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	}

	// add to conns lists
	insertElement(id1->connections, id2);
	insertElement(id2->connections, id1);

	// unlock conns mutex in both channels
	int val = 0;
	sem_getvalue(&(id1->conns_sem), &val);
	if(val == 0) {	// never allow semaphore to go above 1; make it act like a mutex
		sem_post(&(id1->conns_sem));
	}

	sem_getvalue(&(id2->conns_sem), &val);
	if(val == 0) {
		sem_post(&(id2->conns_sem));
	}
	
	pthread_mutex_unlock(&(id1->mutex));
	pthread_mutex_unlock(&(id2->mutex));

	pthread_mutex_unlock(&conn_op_mutex);
	return true;
}

void channel_unbind(Channel_PNTR id) {
	pthread_mutex_lock(&conn_op_mutex);

	// iterate through list, locking then disconnecting
	unsigned int length = getListLength(id->connections);
	Channel_PNTR opposite; // channel on the opposite side of current connection
	int i;
	for(i = 0; i < length; i++) {
		opposite = getElementN(id->connections, i);	// fetch current opposite half-channel

		sem_wait(id->direction == CHAN_IN ? &(id->conns_sem) : &(opposite->conns_sem) );
		sem_wait(id->direction == CHAN_IN ? &(opposite->conns_sem) : &(id->conns_sem) );
		pthread_mutex_lock(id->direction == CHAN_IN ? &(id->mutex) : &(opposite->mutex) );
		pthread_mutex_lock(id->direction == CHAN_IN ? &(opposite->mutex) : &(id->mutex) );

		removeElement(id->connections, opposite);
		removeElement(opposite->connections, id);

		pthread_mutex_unlock(&(id->mutex));
		pthread_mutex_unlock(&(opposite->mutex));

		if(!isEmpty(id->connections))
		{
			sem_post(&(id->conns_sem));
		}
		if(!isEmpty(opposite->connections))
		{
			sem_post(&(opposite->conns_sem));
		}
		length = getListLength(id->connections);
	}

	pthread_mutex_unlock(&conn_op_mutex);
	return;
}

int channel_select(struct select_struct *s) {
	return 0;
}

int channel_send(Channel_PNTR id, void *data, void *ex_handler) {
	sem_wait(&(id->conns_sem));
	pthread_mutex_lock(&(id->mutex));

	id->buffer = data;
	id->ready = true;
	pthread_mutex_unlock(&(id->mutex));

	// iterate through connection list, looking for receiver that is ready
	unsigned int length = getListLength(id->connections);
	Channel_PNTR match; // current receiver
	int i;
	for(i = 0; i < length; i++) {
		match = getNextElement(id->connections);	// fetch next channel in conns list (which keeps state across calls)

		pthread_mutex_lock(&(match->mutex));
		pthread_mutex_lock(&(id->mutex));

		if(match->ready && id->ready) {
			match->buffer = id->buffer;
			match->ready = false;
			id->ready = false;
			id->nd_received = true;

			sem_post(&(match->blocked));
			pthread_mutex_unlock(&(id->mutex));
			pthread_mutex_unlock(&(match->mutex));
			sem_wait(&(match->actually_received));


			int val = 0;
			sem_getvalue(&(id->conns_sem), &val);
			if(val == 0) {
				sem_post(&(id->conns_sem));
			}
			return 0;
		}

		pthread_mutex_unlock(&(id->mutex));
		pthread_mutex_unlock(&(match->mutex));
	}

	int val = 0;
	sem_getvalue(&(id->conns_sem), &val);
	if(val == 0) {
		sem_post(&(id->conns_sem));
	}
	sem_wait(&(id->blocked));	// blocked should always be locked, so wait here until data is taken by active part of a receive

	return 0;
}

int channel_receive(Channel_PNTR id, void *data, bool in_ack_after) {
	sem_wait(&(id->conns_sem));
	pthread_mutex_lock(&(id->mutex));

	id->ready = true;

	pthread_mutex_unlock(&(id->mutex));

	// iterate through connection list, looking for receiver that is ready
	unsigned int length = getListLength(id->connections);
	Channel_PNTR match; // current receiver
	int i;
	for(i = 0; i < length; i++) {
		match = getNextElement(id->connections);	// fetch next channel in conns list (which keeps state across calls)

		pthread_mutex_lock(&(id->mutex));
		pthread_mutex_lock(&(match->mutex));

		if(match->ready && id->ready) {
			id->buffer = match->buffer;		// found a ready sender, get pointer
			memcpy(data, id->buffer, id->typesize);	// got pointer from sender; copy data

			match->ready = false;
			id->ready = false;

			sem_post(&(match->blocked));
			pthread_mutex_unlock(&(match->mutex));
			pthread_mutex_unlock(&(id->mutex));

			int val = 0;
			sem_getvalue(&(id->conns_sem), &val);
			if(val == 0) {
				sem_post(&(id->conns_sem));
			}
			return 0;
		}

		pthread_mutex_unlock(&(match->mutex));
		pthread_mutex_unlock(&(id->mutex));
	}

	int val = 0;
	sem_getvalue(&(id->conns_sem), &val);
	if(val == 0) {
		sem_post(&(id->conns_sem));
	}
	sem_wait(&(id->blocked));	// wait here until data is ready in active part of a send

	memcpy(data, id->buffer, id->typesize);	// receiver now has pointer; copy data

	sem_post(&(id->actually_received));

	return 0;
}