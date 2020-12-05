//
// Created by hanyuan on 12/1/20.
//

#ifndef CS537_P4_PAGETABLE_H
#define CS537_P4_PAGETABLE_H
#include <stdlib.h>

typedef struct Statistic {
    ulong TPI; // total page faults
    ulong TMR; // total mem ref
    ulong CRP;  // current Running Processes
    ulong TRP; // total runnable processes, sum of each tick's data
    // AMU = TMU/(TPF*RT)
    ulong TPF; // total page frames, realMemSize/pageSize
    ulong TMU; // total page frames occupied, sum of each tick's data
    ulong CMU;
    ulong RT; // total ticks
} Statistic;

typedef struct PageTableEntry{
    ulong pid;
    ulong vpn;
    ulong useTime;
    ulong freeBit;
} PageTableEntry;

int entryCompare(const void* a, const void* b);
PageTableEntry* getEntry(void** root, ulong pid, ulong vpn);
PageTableEntry* addEntry(void** root, PageTableEntry* entry);
void deleteEntry(void** root, PageTableEntry* entry);






#endif //CS537_P4_PAGETABLE_H
