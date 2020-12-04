//
// Created by hanyuan on 12/1/20.
//

#include "process.h"
#include <search.h>
#include <stdio.h>
#define _GNU_SOURCE

/**
 * normal enqueue action
 * @param value the node value
 * @param queue the queue to work on
 */
void enQueue(Process* process, Queue* queue){
    if(queue->first == NULL){
        queue->first = (QueueNode*)malloc(sizeof(QueueNode));
        queue->first->process = process;
        queue->last = queue->first;
    }
    else{
        queue->last->next = (QueueNode*)malloc(sizeof(QueueNode));
        queue->last = queue->last->next;
        queue->last->process = process;
    }
}

/**
 * normal dequeue action (and free the dequeued node)
 * @param queue the queue to work on
 * @return the dequeued node's process
 */
Process* deQueue(Queue* queue){
    if(queue->first == NULL){
        fprintf(stderr, "Attemp to dequeue an empty queue\n");
        exit(1);
    }
    QueueNode* buf = queue->first;
    Process* res = buf->process;
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

/**
 * return 1 if Process a has larger pid, return -1 if b has larger pid, return 0 if same pid
 * @param a one Process
 * @param b another Process
 * @return see above
 */
int processCompare(const void* a, const void* b){
    Process* pA = (Process*)a;
    Process* pB = (Process*)b;
    if(pA->pid > pB->pid) return 1;
    else if(pA->pid < pB->pid) return -1;
    else return 0;

}

Process* getProcess(Process** root, ulong pid){
    Process* res;
    void* buf;
    Process keyProcess;
    keyProcess.pid = pid;
    if((buf = tfind(&keyProcess, (void**)root, processCompare)) == NULL){
        return NULL;
    }
    res = (Process*) buf;
    return res;
}

/**
 * Add a Process to the tree from root and return it
 * if it already exists then return the one in the tree
 * @param root the tree's root
 * @param Process to be added
 * @return the
 */
Process* addProcess(Process** root, Process* process){
    ulong pid = process->pid;
    void* buf;
    Process* res;
    if((res = getProcess(root, pid)) != NULL){
        return res;
    }
    else{
        if((buf = tsearch(process, (void**)root, processCompare)) == NULL){
            fprintf(stderr, "Unable to add node\n");
            exit(1);
        }
        res = (Process*) buf;
        return res;
    }
}




