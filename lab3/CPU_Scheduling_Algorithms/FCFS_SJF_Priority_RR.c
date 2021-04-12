// CPU-Scheduling-Algorithms-In-C

// 1. FCFS
// 2. SJF
// 3. Priority
// 4. RR

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include<malloc.h>

#define SIZE 100
#define n 5   // no.of processes
#define qt 10 // time quantum for RR

void bubble_sort_SJF(int *bt, int *p); 
void bubble_sort_Priority(int *bt, int *p, int *pr);
void RR(int *bt, int *wt, int *tat);
void gantt_chart_FCFS_SJF_Priority(int *wt, int *p, int *tat);
void wt_tat_FCFS_SJF_Priority(int *bt, int *wt, int *tat);

//Calculates wait time, turn around time for FCFS, SJF, Priority Scheduling
void wt_tat_FCFS_SJF_Priority(int *bt, int *wt, int *tat)
{
    int i;
    
    wt[0] = 0;
    tat[0] = bt[0];
    for(i=1; i<n; i++)
    {
        wt[i] = wt[i-1] + bt[i-1];  //waiting time[p] = waiting time[p-1] + Burst Time[p-1]
        tat[i] = wt[i] + bt[i];     //Turnaround Time = Waiting Time + Burst Time
    }

}



// RR Scheduling Algorithm
void RR(int *bt, int *wt, int *tat)
{
    int i, temp_n = n, count, initial_time, terminal_time, flag = 0;
    
    int *temp_bt;  // temp bt to make changes
    temp_bt = (int*)malloc(n*sizeof(int));
    
    
    
    for(i=0; i<n; i++)
    {
        temp_bt[i] = bt[i];
    }


    wt[0] = 0;
    printf("\n GAANT CHART \n");
    printf("\n-------------\n");
    
    // iterating through processes in multiple passes...
    for(terminal_time=0, count=0; temp_n!=0;) {
        initial_time = terminal_time;
        
        // when process has completed its burst...
        if(temp_bt[count] <= qt && temp_bt[count] > 0) {  
            terminal_time += temp_bt[count];
            temp_bt[count] = 0;

	    // Updating Performance parameters only when burst is completed
            wt[count] = terminal_time - bt[count];     // 
            tat[count] = wt[count] + bt[count];
            flag = 1;
        }
        
        // when process needs more than one time quantums...
        else if(temp_bt[count] > qt) {  
            temp_bt[count] -= qt;
            terminal_time += qt;
        }
        
        //when process burst completes  
        if(temp_bt[count] == 0 && flag == 1) {   
            temp_n--;   // one process is completed 
            flag=0;
        }
        
        //Gaant chart
        if(initial_time != terminal_time) {
            printf(" %d\t|| P%d ||\t%d\n", initial_time, count, terminal_time);
        }
        
        
        if(count == n-1)
            count = 0;  // traverser for multiple passes
        else
            ++count;
    }
    
    
}


// Sorts processes in order of decreasing priority for Priority Scheduling Algorithm
void bubble_sort_Priority(int *bt, int *p, int *pr)
{
    int i, j, pos, temp;
    
    for (i=0; i<n; i++)
    {
        pos = i;
        for (j=i+1; j<n; j++)
        {
            if (pr[j] < pr[pos])
                pos = j; // swapping the positions
          
        }
        
        // swapping the values
        temp = pr[i];
        pr[i] = pr[pos];
        pr[pos] = temp;
        
        temp = bt[i];
        bt[i] = bt[pos];
        bt[pos] = temp;
        
        temp = p[i];
        p[i] = p[pos];
        p[pos] = temp;
        
    }
}


// Sorts processes in order of increasing burst times for SJF Scheduling Algorithm
void bubble_sort_SJF(int *bt, int *p)
{
    int i, j, pos, temp;
    
    for (i=0; i<n; i++)
    {
        pos = i;
        for (j=i+1; j<n; j++)
        {
            if (bt[j] < bt[pos])
                pos = j; // swapping the positions
          
        }
        
        // swapping the values
        temp = bt[i];
        bt[i] = bt[pos];
        bt[pos] = temp;
        
        temp = p[i];
        p[i] = p[pos];
        p[pos] = temp;
        
    }
}


// Draws Gaant Chart for FCFS, SJF, Priority Algorithms
void gantt_chart_FCFS_SJF_Priority(int *wt, int *p, int *tat)
{
    int i;
    printf("\n GAANT CHART \n");
    printf("---------------\n");
    for(i=0; i<n; i++)
    {
        printf(" %d\t|| P%d ||\t%d\n", wt[i], p[i], tat[i]);
    }
}



//


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
    
    
    // Pick the sorting Algorithm for SJF, Priority Scheduling...
    
    //bubble_sort_Priority(bt, p, pr);
    //bubble_sort_SJF(bt, p);


    RR(bt, wt, tat);





// waiting time and tat

    //wt_tat_FCFS_SJF_Priority(bt, wt, tat);




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




// GANTT CHART
    //gantt_chart_FCFS_SJF_Priority(wt, p, tat);
    
}
