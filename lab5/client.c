//
// Created by mohsin on 15/5/21.
//

#include <stdio.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};


void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}



int main(void)
{
    // create some work to do
    struct data work1;
    work1.a = 5;
    work1.b = 10;

    // create some work to do
    struct data work2;
    work2.a = 15;
    work2.b = 20;

    // Create some more work to do
    struct data work3;
    work3.a = 30;
    work3.b = 10;

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    pool_submit(&add,&work1);

    pool_submit(&add,&work2);

    pool_submit(&add,&work3);

    // may be helpful
    //sleep(3);

    pool_shutdown();

    return 0;
}