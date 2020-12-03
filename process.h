//
// Created by hanyuan on 12/1/20.
//

#ifndef CS537_P4_PROCESS_H
#define CS537_P4_PROCESS_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
    ulong pid;
    ulong curr;
    ulong last;
} Process;

Process* addProcess(Process** root, ulong pid);
Process* getProcess(ulong pid);

ulong getCurr(ulong pid);

#endif //CS537_P4_PROCESS_H
