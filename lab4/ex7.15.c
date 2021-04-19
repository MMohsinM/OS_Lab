#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

u_int32_t buffer[BUFFER_SIZE];
pthread_mutex_t lock;

void *primes(void *param)
{
    pthread_mutex_lock(&lock);
    int *p = param;
    int n = *p;

    buffer[0] = 0;
    buffer[1] = 1;
    for (int i = 2; i < n; i++)
    {
        buffer[i] = buffer[i - 1] + buffer[i - 2];
    }
    
    pthread_mutex_unlock(&lock);
    pthread_exit(0);
    
    
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_cond_t cond_var;
    int n, error;
    scanf("%d", &n);
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    if (n >= 48)
    {
        printf("input %d would be overflow, the overflowed result is shown anyway\n", n);
    }
    // get thread attributes
    pthread_attr_init(&attr);
    pthread_cond_init(&cond_var,NULL);

    // create a thread
    error=pthread_create(&tid, &attr, primes, &n);

    if (error !=0)
    	printf(" \n Thread cannot be created");
    
    while (error!=0)
    	pthread_cond_wait(&cond_var, &lock);
    	error+=1;
    	
    // wait for each thread to finish
    pthread_join(tid, NULL);

    for (int i = 0; i < n; i++)
    {
        printf("%d ", buffer[i]);
    }
    pthread_mutex_destroy(&lock);
    printf("\n");
}
