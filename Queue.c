//
// Created by hanyuan on 12/2/20.
//

#include "Queue.h"
#include <stdio.h>

/**
 * normal enqueue action
 * @param value the node value
 * @param queue the queue to work on
 */
void enQueue(ulong value, Queue* queue){
    if(queue->first == NULL){
        queue->first = (QueueNode*)malloc(sizeof(QueueNode));
        queue->first->value = value;
        queue->last = queue->first;
    }
    else{
        queue->last->next = (QueueNode*)malloc(sizeof(QueueNode));
        queue->last = queue->last->next;
        queue->last->value = value;
    }
}

/**
 * normal dequeue action (and free the dequeued node)
 * @param queue the queue to work on
 * @return the dequeued node's value
 */
ulong deQueue(Queue* queue){
    if(queue->first == NULL){
        fprintf(stderr, "Attemp to dequeue an empty queue\n");
        exit(1);
    }
    QueueNode* buf = queue->first;
    ulong res = buf->value;
    if(queue->last == queue->first){
        queue->last = queue->last->next;
    }
    queue->first = queue->first->next;
    free(buf);
    buf = NULL;
    return res;
}

/**
 * Free the queue and all its nodes
 * @param queue to be freed
 */
void destoryQueue(Queue* queue){
    while(queue->first != NULL){
        QueueNode* buf = queue->first;
        queue->first = queue->first->next;
        free(buf);
        buf = NULL;
    }
    free(queue);
}

