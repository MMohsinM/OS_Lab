/* START: fig3_57.txt */
#ifndef _Queue_h
#define _Queue_h

struct QueueRecord;
typedef struct QueueRecord *Queue;


int IsEmptyQueue(Queue Q );
Queue CreateQueue( void );
void DisposeQueue( Queue Q );
void MakeEmpty( Queue Q );
void Enqueue(ElementType data, Queue Q);
void Front(ElementType data, Queue Q);
void * Dequeue( Queue Q );


#endif  /* _Queue_h */
/* END */