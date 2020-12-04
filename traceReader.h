//
// Created by hanyuan on 12/1/20.
//

#ifndef CS537_P4_TRACEREADER_H
#define CS537_P4_TRACEREADER_H
#include "537pfsim.h"

typedef struct Statistic {
    ulong TPI; // total page faults
    ulong TMR; // total mem ref
    ulong CRP;  // current Running Processes
    ulong TRP; // total runnable processes, sum of each tick's data
    // AMU = TMU/(TPF*RT)
    ulong TPF; // total page frames, realMemSize/pageSize
    ulong TMU; // total page frames occupied, sum of each tick's data
    ulong RT; // total ticks

} Statistic;

void parseTrace(const char* tracefile, ulong pageSize, ulong realMemSize);
int readPair(ulong* first, ulong* second, const char* const rawStr);

void loadProcess(FILE* fp, Statistic* sta, Process** root);
#endif //CS537_P4_TRACEREADER_H
