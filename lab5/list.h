//
// Created by mohsin on 15/5/21.
//

#ifndef UNTITLED2_LIST_H
#define UNTITLED2_LIST_H



struct Node;
typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
List MakeEmptyList(List L);
int IsEmptyList(List L);
int IsLast(Position P, List L);
Position Find(ElementType X, List L);
void Delete(ElementType X, List L);
void DeleteByAddress(Position X, List L);
Position FindPrevious(ElementType X, List L);
Position FindPreviousByAddress(Position X, List L);
Position Insert(ElementType X, List L, Position P);
void DeleteList(List L); Position Header(List L);
ElementType DequeueList(List L);
Position First(List L);
Position Advance(Position P);
ElementType Retrieve(Position P);
ElementType FrontList(List L);
int GetSizeOfNode(void);

#endif //UNTITLED2_LIST_H
