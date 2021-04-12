

// CPU-Scheduling-Algorithms-In-C

// Periority based RR  

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include<malloc.h>


#define SIZE 100
#define n 5
#define qt 10



void bubble_sort_Priority(int *bt, int *p, int *pr);
int RR(int *bt, int *bt_temp, int *wt, int *tat, int counter, int pos, int initial_time, int terminal_time, int *p);



// Sorts processes in order of decreasing priority for Priority Scheduling Algorithm
void bubble_sort_Priority(int *bt, int *p, int *pr){
    int i, j, pos, temp;
    
    for (i=0; i<n; i++)
    {
        pos = i;
        for (j=i+1; j<n; j++)
        {
            if (pr[j] < pr[pos])
                pos = j;  // swapping the positions
          
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





// RR Scheduling Algorithm
int RR(int *bt, int *bt_temp, int *wt, int *tat, int counter, int pos, int initial_time, int terminal_time, int *p)
{
    int i, temp_n = counter, count, flag = 0;
    
    int *temp_bt;  // temp bt to make changes
    temp_bt = (int*)malloc(counter*sizeof(int));
    
    
    
    for(i=0; i<counter; i++)
    {
        temp_bt[i] = bt_temp[i];
       
    }


    

    for(count=0; temp_n!=0;) {
        initial_time = terminal_time;
        
        // FCFS when last process is there... 
        if (temp_bt[count] != 0 && temp_n == 1){
        
            terminal_time += temp_bt[count];
            
            wt[count+pos] = terminal_time - bt[count+pos];     // 
            tat[count+pos] = wt[count+pos] + bt[count+pos];
            
            printf("\nInto FCFS\n");
            
            printf(" %d\t|| P%d ||\t%d\n", initial_time, p[pos+count], terminal_time);
                
            break;
        }
        
        
         // when process has completed its burst...
        if(temp_bt[count] <= qt && temp_bt[count] > 0) {  
            terminal_time += temp_bt[count];
            temp_bt[count] = 0;

	    // Updating Performance parameters only when burst is completed
            wt[count+pos] = terminal_time - bt[count+pos];     // 
            tat[count+pos] = wt[count+pos] + bt[count+pos];
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
            printf(" %d\t|| P%d ||\t%d\n", initial_time, p[pos+count], terminal_time);
        }
        
        
        
        if(count == counter-1)
            count = 0;  // traverser for multiple passes of RR
        else
            ++count;
    }
    
    return terminal_time;
}






//

void main(int argc, char *argv[]) 


{
    int i, j, counter, k, l;
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
	
        //add(name,priority,burst);
	
	
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
    
    
    bubble_sort_Priority(bt, p, pr);
    
    
    
    printf("\n GAANT CHART \n");
    printf("\n-------------\n");
    
    int initial_time, terminal_time = 0;
    for (i=0; i<n;){
        initial_time = terminal_time;
        counter=1;
        for (j=i+1; j<n; j++){
            if (pr[j] == pr[i])
                counter++;
                    
        }
        
        if (counter == 1){  // Priority based FCFS
            printf("\nInto FCFS\n");
            terminal_time += bt[i];
            
            
            wt[i] = terminal_time - bt[i];     // 
            tat[i] = wt[i] + bt[i];
            
            printf(" %d\t|| P%d ||\t%d\n", initial_time, p[i], terminal_time); 
            
            i++;
        }
        else{   // Priority based RR
            
            printf("\nInto RR\n");
            
            int *bt_temp;
            bt_temp = (int*)malloc(counter*sizeof(int)); //for RR
            int l=0;
            for (k=i; k<i+counter; k++)
            {
                bt_temp[l] = bt[k];
                l++;
            }
            
            terminal_time = RR(bt, bt_temp, wt, tat, counter, i, initial_time, terminal_time, p);
            
            
            i+=counter;
            
        }
        
        
}


// avg. waiting time and avg. turn around time

    for(i=0; i<n; i++)
    {
        avgwt += wt[i];
        avgtat += tat[i]; 
    }
    avgwt = avgwt/n;
    avgtat = avgtat/n;



//


    printf("\n\n PROCESS \t PRIORITY\t BURST TIME \t WAITING TIME \t TURNAROUND TIME \n");
    printf("--------------------------------------------------------------\n");
    for(i=0; i<n; i++)
    {
        printf(" P%d \t\t %d \t\t %d \t\t %d \t\t %d \n", p[i], pr[i], bt[i], wt[i], tat[i]);
    }

    printf("\n Average Waiting Time = %f \n Average Turnaround Time = %f \n", avgwt, avgtat);

    
}
