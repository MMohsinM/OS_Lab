#include "task.h"
#include "list.h"
#include "fatal.h"
#include <stdlib.h>


/* Place in the interface file */

struct Node
{
    struct Node *Next;
    ElementType* Element;
};

int GetSizeOfNode(){
    return sizeof( struct Node);
}

List MakeEmptyList(List L) {
    if (L != NULL)
        DeleteList(L);
    L = (List)malloc(sizeof(struct Node));
    if (L == NULL)
        FatalError("Out of memory!");
    L->Next = NULL;
    return L;
}
/* START: fig3_8.txt */
/* Return true if L is empty */
int
IsEmptyList(List L)
{
    return L->Next == NULL;
}
/* END */


/* START: fig3_9.txt */
/* Return true if P is the last position in list L */
/* Parameter L is unused in this implementation */
int IsLast(Position P, List L)
{
    return P->Next == NULL;
}
/* END */


/* START: fig3_10.txt */
/* Return Position of X in L; NULL if not found */
//Position
//Find(ElementType X, List L)
//{
//    Position P;
//    /* 1*/ P = L->Next;
//    /* 2*/ while (P != NULL && P->Element != X)
//        /* 3*/ P = P->Next;
//    /* 4*/ return P;
//}
/* END */
/* START: fig3_11.txt */
/* Delete from a list */
/* Cell pointed to by P->Next is wiped out */
/* Assume that the position is legal */
/* Assume use of a header node */
void
Delete(ElementType X, List L)
{
    Position P, TmpCell;
    P = FindPrevious(X, L);
    if (!IsLast(P, L)) /* Assumption of header use */
    { /* X is found; delete it */
        TmpCell = P->Next;
        P->Next = TmpCell->Next; /* Bypass deleted cell */
        free(TmpCell->Element); //Deleting Task Next
        free(TmpCell);
    }
}


ElementType DequeueList(List L) {
    Position TmpCell;
    ElementType t;
    if (!IsEmptyList(L))
    {
        TmpCell = L->Next; // Tmp points to 1st Node
        L->Next = L->Next->Next; // Header Next points to 2nd node
        //free(TmpCell->Element); // Free the element as well
        t = TmpCell->Element;
        free(TmpCell); // Freeing 1st node
        return t;
    }
    else {
        puts("Queue is Empty");
        return NULL;
    }
}



/* END */
/* START: fig3_12.txt */
/* If X is not found, then Next field of returned value is NULL */
/* Assumes a header */
Position
FindPrevious(ElementType X, List L)
{
    Position P;
    /* 1*/ P = L;
    /* 2*/ while (P->Next != NULL && P->Next->Element != X)
        /* 3*/ P = P->Next;
    /* 4*/ return P;
}
/* END */
/* START: fig3_13.txt */
/* Insert (after legal position P) */
/* Header implementation assumed */
/* Parameter L is unused in this implementation */
Position
Insert(ElementType X, List L, Position P)
{
    Position TmpCell;
    /* 1*/ TmpCell = (Position)malloc(sizeof(struct Node));
    /* 2*/ if (TmpCell == NULL)
        /* 3*/ FatalError("Out of space!!!");
    /* 4*/ TmpCell->Element = X;
    /* 5*/ TmpCell->Next = P->Next;
    /* 6*/ P->Next = TmpCell;
    return TmpCell;
}
/* END */
#if 0
/* START: fig3_14.txt */
 /* Incorrect DeleteList algorithm */
void
DeleteList(List L)
{
	Position P;
	/* 1*/ P = L->Next; /* Header assumed */
	/* 2*/ L->Next = NULL;
	/* 3*/ while (P != NULL)
	{
		/* 4*/ free(P);
		/* 5*/ P = P->Next;
	}
}
/* END */
#endif
/* START: fig3_15.txt */
/* Correct DeleteList algorithm */
void
DeleteList(List L)
{
    Position P, Tmp;
    /* 1*/ P = L->Next; /* Header assumed */
    /* 2*/ L->Next = NULL;
    /* 3*/ while (P != NULL)
    {
        /* 4*/ Tmp = P->Next;
        free(Tmp->Element); // Delete the task node as well
        /* 5*/ free(P);
        /* 6*/ P = Tmp;
    }
}
/* END */
Position
Header(List L)
{
    return L;
}
Position
First(List L)
{
    return L->Next;
}
Position
Advance(Position P)
{
    return P->Next;
}
ElementType
Retrieve(Position P)
{
    return P->Element;
}

ElementType FrontList(List L){
    if (!IsEmptyList(L)) {
        return L->Next->Element;
    } else {
        return NULL;
    }
}