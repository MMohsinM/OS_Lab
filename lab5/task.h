//
// Created by mohsin on 15/5/21.
//

#ifndef UNTITLED2_TASK_H
#define UNTITLED2_TASK_H

typedef struct
{
    void (*function)(void *p);
    void *data;
}
        Task;

typedef Task* ElementType;


#endif //UNTITLED2_TASK_H
