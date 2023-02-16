#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t waiting_queue;
	int count;
};

sem_t sem_create(size_t count)
{
	sem_t temp = malloc(sizeof(struct semaphore));
	if (temp == NULL) {
		return temp;
	}
	temp->waiting_queue = queue_create();
	temp->count = count;
	return temp;
}

int sem_destroy(sem_t sem)
{
	queue_destroy(sem->waiting_queue);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	if (sem->count > 0) {
		sem->count--;
	} else {
		struct uthread_tcb *temp = uthread_current();
		queue_enqueue(sem->waiting_queue, temp);
		uthread_block();
	}
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	struct uthread_tcb *temp = uthread_current();
	struct uthread_tcb *next;
	sem->count++;
	if (queue_length(sem->waiting_queue) != 0) {
		queue_dequeue(sem->waiting_queue, (void**)&next);
		uthread_unblock(next);
	}
	return 0;
}

