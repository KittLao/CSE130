
#include "cartman.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t juncs[5];

struct arg_struct {
	unsigned int c;
	enum track t;
	enum junction j;
};

int get_other_junc(unsigned int cart, enum track track, enum junction junction) {
	if((int)junction == (int)track)
		return (int)((junction + 1) % 5);
	return (int)track;
}

void* thread_func(void* arg) {
	struct arg_struct *args = arg;
	int other_junc = get_other_junc(args->c, args->t, args->j);

	sem_wait(&juncs[args->j]);
	sem_wait(&juncs[other_junc]);
	reserve(args->c, args->j);
	reserve(args->c, other_junc);

	cross(args->c, args->t, args->j);
	free(args);
	return NULL;
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
	pthread_t thread;
	struct arg_struct *args = (struct arg_struct*)malloc(sizeof(struct arg_struct));
	args->c = cart;
	args->t = track;
	args->j = junction;
	pthread_create(&thread, NULL, thread_func, (void*)args);
	pthread_join(thread, NULL);
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void depart(unsigned int cart, enum track track, enum junction junction) 
{
	int other_junc = get_other_junc(cart, track, junction);
	release(cart, other_junc);
	release(cart, junction);
	sem_post(&(juncs[other_junc]));
	sem_post(&(juncs[junction]));
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void cartman() 
{
	for(int i = 0; i < 5; i++) {
		sem_init(&juncs[i], 0, 1);
	}
}

