//
// Created by hanyuan on 12/1/20.
//

#ifndef CS537_P4_PROCESS_H
#define CS537_P4_PROCESS_H
#include <stdio.h>
#include <stdlib.h>
#include "pageTable.h"

enum IOtick {IOtick = 2000000};

typedef struct Process {
    ulong pid;
    ulong curr; // current FP
    ulong currVpn;
    ulong last;
    ulong waiting;
    ulong started;
} Process;

Process* endProcess(Process** root, Process* process);
Process* addProcess(Process** root, Process* process);
Process* getProcess(Process** root, ulong pid);

// IO queue for processes
typedef struct QueueNode{
    struct QueueNode* next;
    Process* process;
} QueueNode;

typedef struct Queue{
    struct QueueNode* first;
    struct QueueNode* last;
    ulong counter;
} Queue;

void enQueue(Process* process, Queue* queue);
Process* deQueue(Queue* queue);
void destoryQueue(Queue* queue);



#endif //CS537_P4_PROCESS_H
