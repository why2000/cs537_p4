//
// Created by hanyuan on 12/1/20.
// Team member: Hanyuan Wu, Zhihao Shu

#ifndef CS537_P4_TRACEREADER_H
#define CS537_P4_TRACEREADER_H
#include "PageAlgorithm.h"
#include "537pfsim.h"
#include "pageTable.h"
#include "process.h"




void parseTrace(const char* tracefile, ulong pageSize, ulong realMemSize);
int readPair(ulong* first, ulong* second, const char* rawStr);

void loadProcess(FILE* fp, Statistic* sta, void** root);
void outputStatistic(const char* tracefile, const Statistic* sta);
#endif //CS537_P4_TRACEREADER_H
