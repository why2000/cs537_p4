//
// Created by hanyuan on 12/4/20.
//

#ifndef CS537_P4_PAGEALGORITHM_H
#define CS537_P4_PAGEALGORITHM_H
#include "pageTable.h"
#include "traceReader.h"
#include "process.h"


void loadEntry(void** root, ulong pid, ulong vpn, Statistic* sta);

#endif //CS537_P4_PAGEALGORITHM_H
