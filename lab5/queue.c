#include "task.h"
#include "list.h"
#include "queue.h"
#include "fatal.h"
#include <stdlib.h>

//#define MinQueueSize ( 5 )


struct QueueRecord
{
    Position Front;
    Position Rear;
    List nested_list_header;  // modified array to store struct in queue
};


/* START: fig3_58.txt */
int
IsEmptyQueue(Queue Q )
{
    return Q->Front == NULL;
}
/* END */


Queue CreateQueue( void)
{
    Queue Q;
    // Malloc a Queue Record
    Q = ( struct QueueRecord* ) malloc( sizeof( struct QueueRecord ) );
      if( Q == NULL )
          FatalError( "Out of space!!!" );
      // Maloc The nested list header ElementType
    Q -> nested_list_header = (List) malloc( GetSizeOfNode() );
    if( Q->nested_list_header == NULL )
          FatalError( "Out of space!!!" );
    MakeEmpty( Q );
    return Q;
}

/* START: fig3_59.txt */
void
MakeEmpty( Queue Q )
{
    Q->Front = NULL;
    Q->Rear = Q->nested_list_header; // To Ensure Enqueue Works Properly
    MakeEmptyList(Q->nested_list_header);
}
/* END */

void
DisposeQueue( Queue Q )
{
    if( Q != NULL )
    {
        DeleteList(Q->nested_list_header);
        free( Q );
    }
}

/* START: fig3_60.txt */


void
Enqueue( ElementType X, Queue Q ) /*Now takes both age and roll*/
{
    int Check = IsEmptyQueue(Q);//This is used to check if Queue is empty
    //Check = 1;  // Normally only rear would be change but in this case
    // after addition Rear And Front would become equal
    Q->Rear = Insert(X, Q->nested_list_header, Q->Rear);
    if (Check)
        Q->Front = First(Q->nested_list_header);
}
/* END */



void
Front(ElementType X, Queue Q) /*Now takes both age and roll
																and returns value in pointers*/
{
    if (!IsEmptyQueue(Q)) {
        X = FrontList(Q->Front);
    }
    else
        Error( "Empty queue" );
    /* Return value used to avoid warning */
}

void * Dequeue(Queue Q) {
    Position TmpCell;
    ElementType X;
    int Check=0;              //Normally DequeueList just changes the front pointer
    if (Q->Front == Q->Rear) {//If there is only one element in the Queue then the Front and Rear
        // Both need to be changed
        Check = 1;
    }
    if (!IsEmptyQueue(Q))
    {
        X = DequeueList(Q->nested_list_header);
        Q->Front = First(Q->nested_list_header);
        if (Check)
            Q->Rear = Q->nested_list_header;
        return X;
    }
    else {
        puts("Queue is Empty");
        return NULL;
    }
}
