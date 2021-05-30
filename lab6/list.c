#include "list.h"
#include "fatal.h"
#include <stdlib.h>


/* Place in the interface file */
struct Node;
typedef struct Node * Position;
int IsEmptyList(List L);
void DeleteList(List L); Position Header(List L);


struct Node
{
    struct Node *Next;
    ElementType Element;
};



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





ElementType Pop(List L) {
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
        //puts("List is Empty");
        return -1;
    }
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

void Push(ElementType x, List L){
    Insert(x, L, L);
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
