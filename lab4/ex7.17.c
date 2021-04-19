#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

int hits = 0;

pthread_mutex_t mutex;

double frand()
{
    return (double)rand() / (double)RAND_MAX;
}

void *cal_pi(void *param)
{
    int total = atoi(param);
    double x, y;
    for (int i = 0; i < total; i++)
    {
        x = frand();
        y = frand();

        if ((x * x + y * y) < 1)
        {
            pthread_mutex_lock (&mutex);
            hits++;
            pthread_mutex_unlock (&mutex);
        }
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 2)  //taking the input on compilation of the c file
    {
        printf("should include your argument\n");
        return 1;
    }
    srand(time(NULL));
    pthread_t tid;
    pthread_attr_t attr;
    
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    // get thread attributes
    pthread_attr_init(&attr);

    // create a thread
    pthread_create(&tid, &attr, cal_pi, argv[1]);

    // wait for each thread to finish
    pthread_join(tid, NULL);
    int total = atoi(argv[1]);

    printf("Monte Carlo Pi: %f\n", 4 * (double)hits / (double)total);
}
