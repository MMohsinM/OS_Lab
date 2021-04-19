#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


int hits = 0;   // Global Variable to store child thread results


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
            hits++;
        }
    }
    pthread_exit(0);
}



int main(int argc, char *argv[])
{
    int total = atoi(argv[1]);   // total trials as specified by user
    
    pthread_t tid;

    // create a thread
    pthread_create(&tid, NULL, cal_pi, argv[1]);

    // wait for child thread to finish
    pthread_join(tid, NULL);

    // printing results stored in global variable
    printf("Monte Carlo Pi: %f\n", 4 * (double)hits / (double)total);
}

