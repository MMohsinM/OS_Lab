/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

//#include "task.h"
//#include "list.h"




#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3



pthread_mutex_t mutex;

// semaphores
sem_t empty;
sem_t full;


// the worker bee
pthread_t bee[NUMBER_OF_THREADS];


// the work queue
Task queue[QUEUE_SIZE];
Task *worktodo;





// array indexes
int in = 0;
int out = 0;












// the worker thread in the thread pool
void *worker(void *param)
{
	
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    
    
    
    worktodo = &queue[out];    // take task from queue
    // execute the task
    execute(worktodo->function, worktodo->data);
    
    out = (out+1)%QUEUE_SIZE;
    
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    
    
    pthread_exit(0);
}





/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}




/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    worktodo->function = somefunction;
    worktodo->data = p;
    
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    
   
    queue[in] = *worktodo;  // insert into task queue
    in = (in+1)%QUEUE_SIZE;
    
    pthread_mutex_unlock(&mutex);  
    sem_post(&full);
      


    return 0;
}





// initialize the thread pool
void pool_init(void)
{

    // thread creation
    for (int i=0; i<NUMBER_OF_THREADS; i++)
    {
	pthread_create(&bee[i],NULL,worker,NULL);
    }
    
    
    //mutex lock initialization
    pthread_mutex_init(&mutex, NULL); 
    
    
    //semaphore initialization
    sem_init(&empty, 0, QUEUE_SIZE);
    sem_init(&full, 0, 0);
    

    
}




// shutdown the thread pool
void pool_shutdown(void)
{
     
    // wait for the threads to complete their tasks...
    for (int j=0; j<NUMBER_OF_THREADS; j++)
    {
	pthread_join(bee[j],NULL);
    }
    
    
    //cancel the threads
    
    
    
}
