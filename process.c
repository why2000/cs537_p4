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
        queue->last->next = NULL;
        queue->first->next = NULL;
    }
    else{
        queue->last->next = (QueueNode*)malloc(sizeof(QueueNode));
        queue->last = queue->last->next;
        queue->last->process = process;
        queue->last->next = NULL;
    }
    queue->numIO++;
}

/**
 * normal dequeue action (and free the dequeued node)
 * @param queue the queue to work on
 * @return the dequeued node's process (Null if empty)
 */
Process* deQueue(Queue* queue){
    if(queue->first == NULL){
        return NULL;
    }
    QueueNode* buf = queue->first;
    Process* res = buf->process;
    if(queue->first->next == NULL){
        queue->last = NULL;
    }
    queue->first = queue->first->next;
    //free(buf);
    buf = NULL;
    queue->numIO--;
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

Process* getProcess(void** root, ulong pid){
    Process* res;
    void* buf = NULL;
    Process keyProcess;
    keyProcess.pid = pid;
    if((buf = tfind(&keyProcess, root, processCompare)) == NULL){
        return NULL;
    }
    res = *(Process**) buf;
    return res;
}

/**
 * Add a Process to the tree from root and return it
 * if it already exists then return the one in the tree
 * @param root the tree's root
 * @param Process to be added
 * @return the
 */
Process* addProcess(void** root, Process* process){
    ulong pid = process->pid;
    void* buf = NULL;
    Process* res;
    if((res = getProcess(root, pid)) != NULL){
        return res;
    }
    else{
        if((buf = tsearch(process, root, processCompare)) == NULL){
            fprintf(stderr, "Unable to add node\n");
            exit(1);
        }
        res = *(Process**) buf;
        return res;
    }
}

/**
 * delete a process from the tree from root
 * @param root the tree's root
 * @param process to be deleted
 */
void deleteProcess(void** root, Process* process){
    ulong pid = process->pid;
    if((getProcess(root, pid)) == NULL){
        return;
    }
    else{
        if(tdelete(process, root, processCompare) == NULL){
            fprintf(stderr, "Unable to delete node\n");
            exit(1);
        }
    }
}










