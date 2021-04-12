// CPU-Scheduling-Algorithms-In-C

// Multi-Level Feedback Queue 

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include<malloc.h>


#define SIZE 100
#define n 5


void RR(int *bt, int *wt, int *tat);



// RR Scheduling Algorithm
void RR(int *bt, int *wt, int *tat)
{
    int i, temp_n = n, count, initial_time, terminal_time, flag = 0;
    int pass;
    int qt;
    
    int *temp_bt;  // temp bt to make changes
    temp_bt = (int*)malloc(n*sizeof(int));
    
    
    
    for(i=0; i<n; i++)
    {
        temp_bt[i] = bt[i];
        //terminal_time += bt[i];
    }


    wt[0] = 0;
    printf("\n GAANT CHART \n");
    printf("\n-------------\n");
    


    pass = 1;
    //printf("Pass %d\n",pass);
    qt = 10;
    for(terminal_time=0, count=0; pass<=2;) {
        initial_time = terminal_time;
        if(temp_bt[count] <= qt && temp_bt[count] > 0) {  // when a process has finished burst
            terminal_time += temp_bt[count];
            temp_bt[count] = 0;

	    // only when burst is completed
            wt[count] = terminal_time - bt[count];     // 
            tat[count] = wt[count] + bt[count];
            flag = 1;
        }
        else if(temp_bt[count] > qt) {  // when a process not finished burst
            temp_bt[count] -= qt;
            terminal_time += qt;
        }
        if(temp_bt[count] == 0 && flag == 1) {   // when burst is finished
            temp_n--;   // one process is completed 
            flag=0;
        }
        if(initial_time != terminal_time) { // process with 0 burst time(already finished)
            printf(" %d\t|| P%d ||\t%d\n", initial_time, count, terminal_time);
        }
        if(count == n-1){
            
            printf("Pass %d Completed\n\n",pass);
            pass++;
            qt+=10;
            count = 0;}  // traverser
        else
            ++count;
    }
    
    
    // After 2 passes(Queues), complete processes by FCFS
    for (i=0; i<n; i++){
        initial_time = terminal_time;
        if (temp_bt[i] != 0){
            terminal_time += temp_bt[i];
            
            printf(" %d\t|| P%d ||\t%d\n", initial_time, i, terminal_time);
            
            wt[i] = terminal_time - bt[i];
            tat[i] = wt[i] + bt[i];
        }
    }
    
    printf("Pass 3 Completed\n");
    
}







void main(int argc, char *argv[]) {
    int i;
    int *bt, *pr, *wt, *tat;
    float avgtat, avgwt;

    bt = (int*)malloc(n*sizeof(int));
    pr = (int*)malloc(n*sizeof(int));
    wt = (int*)malloc(n*sizeof(int));
    tat = (int*)malloc(n*sizeof(int));
    
    FILE *in;
    char *temp;
    char task[SIZE];
    
    char *name;
    int priority;
    int burst;

    in = fopen(argv[1],"r");
    
    i = 0;
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks 
	
	pr[i] = priority;
	bt[i] = burst;
	i = i + 1;
	
        free(temp);
    }

    fclose(in);
    
    
      
    int *p;  // positions
    p = (int*)malloc(n*sizeof(int));
    for (i=0; i<n; i++){
        p[i] = i ;
    }
    

// RR with multiple passes..
    RR(bt, wt, tat);



// avg. waiting time and avg. turn around time

    for(i=0; i<n; i++)
    {
        avgwt += wt[i];
        avgtat += tat[i]; 
    }
    avgwt = avgwt/n;
    avgtat = avgtat/n;



//


    printf("\n PROCESS \t PRIORITY\t BURST TIME \t WAITING TIME \t TURNAROUND TIME \n");
    printf("--------------------------------------------------------------\n");
    for(i=0; i<n; i++)
    {
        printf(" P%d \t\t %d \t\t %d \t\t %d \t\t %d \n", p[i], pr[i], bt[i], wt[i], tat[i]);
    }

    printf("\n Average Waiting Time = %f \n Average Turnaround Time = %f \n", avgwt, avgtat);

}

