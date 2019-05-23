#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct node{
	int index;
	struct node* next;
}node_t;

node_t *head = NULL;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void cleanup_func(void *arg)
{
	printf("exec cleanup_func...\n");
	if(arg != NULL)
	{
		free(arg);
	}
	pthread_mutex_unlock(&mtx);
}

void* thread_func(void* arg)
{
	node_t* p = NULL;
	
	pthread_cleanup_push(cleanup_func, p);
	while(1)
	{
		pthread_mutex_lock(&mtx);
		while(head == NULL)
		{
			pthread_cond_wait(&cond, &mtx);			
		}
		p = head;

		printf("thread2:	%d\n", p->index);
		head = head->next;
		free(p);
		pthread_mutex_unlock(&mtx);	
	}
	pthread_cleanup_pop(0);
}

int main(int argc, char** argv)
{
	pthread_t tid;
	node_t *p = NULL;
	pthread_create(&tid, NULL, thread_func, NULL);
 	
	for(int i=0; i<10; i++)
	{
		p = malloc(sizeof(node_t));
		p->index = i;
		pthread_mutex_lock(&mtx);
		p->next = head;
		head = p;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		sleep(1);	
	}
	
	printf("thread1 cancel thread2\n");
	pthread_cancel(tid);

	pthread_join(tid, NULL);
	printf("process exit\n");
	return 0;
}
