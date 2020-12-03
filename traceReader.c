//
// Created by hanyuan on 12/1/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "traceReader.h"
#include "process.h"
#include "Queue.h"

/**
 * Set up the graph and start making
 * @param makeFile the name of makefile
 * @param target the name of target
 */
void parseTrace(const char* const tracefile, const ulong pageSize, const ulong realMemSize){
    FILE* fp;
    char bufLine[MAX_LINE];
    // load tracefile
    if((fp = fopen(tracefile, "r")) == NULL){
        perror("Unable to open tracefile\n");
        exit(1);
    }
    Process* rootProcess;
    loadProcess(fp, &rootProcess);
    fseek(fp, 0, SEEK_SET);
    // value is pid
    Queue* IOQueue;
    if((IOQueue = (Queue*)malloc(sizeof(Queue))) == NULL){
        fprintf(stderr, "Unable to malloc\n");
        exit(1);
    }
    ulong clock = 0;
    ulong lineNum = 0;
    while(fgets(bufLine, MAX_LINE, fp) != NULL){
        lineNum++;
        ulong pid, vpn;
        int lineFlag = readPair(&pid, &vpn, bufLine);
        if(lineFlag == 2) continue;
        if(lineFlag == 1){
            fprintf(stderr, "Error parsing trace file at line %ld\n", lineNum);
            exit(1);
        }
        if(ftell(fp) < getCurr(pid)) continue;
        //if(getPage(pid, vpn) == 0)
        {
            // set pid's fp
            enQueue(pid, IOQueue);
        }
        clock++;


    }
    if(!feof(fp)){
        fprintf(stderr, "Unsupported tracefile format\n");
        exit(1);
    }

}

void loadProcess(FILE* fp, Process** root){
    char bufLine[MAX_LINE];
    ulong lineNum = 0;
    while(fgets(bufLine, MAX_LINE, fp) != NULL){
        ulong pid, vpn;
        int lineFlag = readPair(&pid, &vpn, bufLine);
        if(lineFlag == 2) continue;
        if(lineFlag == 1){
            fprintf(stderr, "Error parsing trace file at line %ld\n", lineNum);
            exit(1);
        }
        Process* curProcess;
        if((curProcess = addProcess(root, pid)) == NULL){
            fprintf(stderr, "Error adding process\n");
            exit(1);
        }
    }
    if(!feof(fp)){
        fprintf(stderr, "Unsupported tracefile format\n");
        exit(1);
    }
}

ulong ticking(Queue* queue){
    if(queue->first != NULL){
        queue->counter++;
    }
    if(queue->counter == IOtick){
        queue->counter = 0;
        return deQueue(queue);
    }
}


/**
 * read a pair of digits out of a string
 * @param target: the buffer to store the item
 * @param rawStr: the string to be parsed
 * @return 0 if read succeeded, 1 if failed, 2 if empty
 */
int readPair(ulong* first, ulong* second, const char* const rawStr){
    ulong a = 0, b = 0;
    ulong i = 0;
    // do a left strip
    while((rawStr[i] == ' ' || rawStr[i] == '\t') && i < MAX_LINE-1)
        i++;
    if(rawStr[i] == '\n' || rawStr[i] == '\0') return 2;
    // read first
    while(rawStr[i] >= '0' && rawStr[i] <= '9'){
        a *= 10;
        a += rawStr[i]-'0';
        if(++i >= MAX_LINE) return 1;
    }
    if(rawStr[i] != ' ' && rawStr[i] != '\t') return 1;
    // lstrip
    while((rawStr[i] == ' ' || rawStr[i] == '\t') && i < MAX_LINE - 1)
        i++;
    // second
    while(rawStr[i] >= '0' && rawStr[i] <= '9'){
        b *= 10;
        b += rawStr[i]-'0';
        i++;
        if(++i >= MAX_LINE) return 1;
    }
    if(rawStr[i] != ' ' && rawStr[i] != '\t' && rawStr[i] != '\n' && rawStr[i] != '\0') return 1;
    // checkline
    while(rawStr[i] == '\t' || rawStr[i] == ' '){
        if(++i >= MAX_LINE) return 1;
    }
    if(rawStr[i] != '\n' && rawStr[i] != '\0') return 1;
    *first = a, *second = b;
    return 0;
}