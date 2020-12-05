//
// Created by hanyuan on 12/1/20.
// Team member: Hanyuan Wu, Zhihao Shu

#ifndef CS537_P4_PAGEALGORITHM_H
#define CS537_P4_PAGEALGORITHM_H
#include "pageTable.h"
#include "process.h"



void loadEntry(void** root, ulong pid, ulong vpn, Statistic* sta);
void endProcess(void** PTRoot, void** processRoot, Process* process, Statistic* sta);

#endif //CS537_P4_PAGEALGORITHM_H
