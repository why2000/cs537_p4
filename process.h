//
// Created by hanyuan on 12/1/20.
// Team member: Hanyuan Wu, Zhihao Shu

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
    ulong ended;
} Process;

int processCompare(const void* a, const void* b);
Process* addProcess(void** root, Process* process);
Process* getProcess(void** root, ulong pid);
void deleteProcess(void** root, Process* process);

// IO queue for processes
typedef struct QueueNode{
    struct QueueNode* next;
    Process* process;
} QueueNode;

typedef struct Queue{
    struct QueueNode* first;
    struct QueueNode* last;
    ulong counter;
    ulong numIO;
} Queue;

void enQueue(Process* process, Queue* queue);
Process* deQueue(Queue* queue);
void destoryQueue(Queue* queue);



#endif //CS537_P4_PROCESS_H
