//
// Created by hanyuan on 12/1/20.
//

#ifndef CS537_P4_TRACEREADER_H
#define CS537_P4_TRACEREADER_H
#include "537pfsim.h"


void parseTrace(const char* tracefile, ulong pageSize, ulong realMemSize);
int readPair(ulong* first, ulong* second, const char* const rawStr);
ulong ticking(Queue* queue, ulong* clock);
void loadProcess(FILE* fp, Process** root);
#endif //CS537_P4_TRACEREADER_H
