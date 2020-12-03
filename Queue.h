//
// Created by hanyuan on 12/2/20.
//

#ifndef CS537_P4_QUEUE_H
#define CS537_P4_QUEUE_H
#include <stdlib.h>
enum IOtick {IOtick  = 2000000};

typedef struct QueueNode{
    struct QueueNode* next;
    ulong value;
} QueueNode;

typedef struct Queue{
    struct QueueNode* first;
    struct QueueNode* last;
    ulong counter;
} Queue;

void enQueue(ulong value, Queue* queue);
ulong deQueue(Queue* queue);
void destoryQueue(Queue* queue);

#endif //CS537_P4_QUEUE_H
