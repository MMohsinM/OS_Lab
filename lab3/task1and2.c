
#include<stdio.h>

void findWaitingTime(int processes[], int n,
						int bt[], int wt[])
{
	// waiting time for first process is 0
	wt[0] = 0;
	
	// calculating waiting time
	for (int i = 1; i < n ; i++ )
		wt[i] = bt[i-1] + wt[i-1] ;
}





void findResponseTime(int processes[], int n,
						int bt[], int wt[])
{
	int arrival[n];
	
	for (int i =0; i<n;i++)
		arrival[i]=0;
	
	// waiting time for first process is 0
	wt[0] = 0;
	
	// calculating response time
	for (int i = 1; i < n ; i++ )	
		wt[i] = bt[i-1] + wt[i-1] -arrival[i];
}

	
// function to calculate turn around time
void findTurnAroundTime( int processes[], int n,
				int bt[], int wt[], int tat[])
{
	// calculating turnaround time by adding
	// bt[i] + wt[i]
	for (int i = 0; i < n ; i++)
		tat[i] = bt[i] + wt[i];
}
	
//function to calculate average time
void findavgTime( int processes[], int n, int bt[])
{
	int wt[n], rt[n], tat[n], total_wt = 0, total_tat = 0;
	
	
	
	//function to find waiting time of all processes
	findWaitingTime(processes, n, bt, wt);
	
	findResponseTime(processes,n,bt,rt);
	
	//function to find turn around time for all processes
	findTurnAroundTime(processes, n, bt, wt, tat);
	
	
	
	
	
	//Display processes along with all details
	printf("Process ID  Burst time      Response Time    Waiting time Turn around time\n");
	
	// Calculate total waiting time and total turn
	// around time
	for (int i=0; i<n; i++)
	{
		total_wt = total_wt + wt[i];
		total_tat = total_tat + tat[i];
		printf("  %d    ",processes[i]);
		printf("	 \t%d ", bt[i] );
		printf("	 \t%d ", rt[i] );  //assuming waiting time is equal to response time as arrival time for each process is zero
		printf("	 \t%d",  wt[i] );
		printf("	 \t%d\n",tat[i] );
	}
	int s=(float)total_wt / (float)n;
	int t=(float)total_tat / (float)n;
	printf("Average waiting time = %d",s);
	printf("\n");
	printf("Average turn around time = %d ",t);
}

void sorttimes(int a[],int n){
int temp;
for (int i=0; i<n;i++){
for (int j=0;j<n;j++){
	if (a[i]<a[j]){
	temp=a[j];
	a[j]=a[i];
	a[i]=temp;
	}

}
	
}
}

// Driver code
int main()
{
	int n;
	int processes[n+1];
	
	
	//Burst time of all processes
	int burst_time[n+1];
	
	
	
	
	
	
	printf("Number of processes: ");
	scanf("%d",&n);
	
	
	for (int i=0;i<n;i++){
	printf("Enter process ID: ");
	scanf("%d",&processes[i]);
	printf("Enter burst time: ");
	scanf("%d",&burst_time[i]);
	}
	
	sorttimes(burst_time,n);   //this function will be called in the case of the SJF algorithm

	
	
	
	findavgTime(processes, n, burst_time);
	return 0;
}


