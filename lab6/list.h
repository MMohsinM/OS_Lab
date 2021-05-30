//
// Created by mohsin on 15/5/21.
//

#ifndef UNTITLED2_LIST_H
#define UNTITLED2_LIST_H


typedef int ElementType;
typedef struct Node* List;
List MakeEmptyList(List L);
ElementType Pop(List L);
void Push(ElementType x, List L);

#endif //UNTITLED2_LIST_H
