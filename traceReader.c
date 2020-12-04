//
// Created by hanyuan on 12/1/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "traceReader.h"
#include "process.h"
#include <search.h>

/**
 * Set up the graph and start making
 * @param makeFile the name of makefile
 * @param target the name of target
 */
void parseTrace(const char* const tracefile, const ulong pageSize, const ulong realMemSize){
    const ulong IOtick = 2000000;
    FILE* fp;
    char bufLine[MAX_LINE];
    // load tracefile
    if((fp = fopen(tracefile, "r")) == NULL){
        perror("Unable to open tracefile\n");
        exit(1);
    }
    Statistic* sta = (Statistic*)malloc(sizeof(Statistic));
    sta->TMU = realMemSize/pageSize;
    Process* rootProcess;
    loadProcess(fp, sta, &rootProcess);
    fseek(fp, 0, SEEK_SET);
    // value is pid
    Queue* IOQueue;
    if((IOQueue = (Queue*)malloc(sizeof(Queue))) == NULL){
        fprintf(stderr, "Unable to malloc\n");
        exit(1);
    }
    while(fgets(bufLine, MAX_LINE, fp) != NULL){
        ulong pid, vpn;
        int lineFlag = readPair(&pid, &vpn, bufLine);
        if(lineFlag == 2) continue;
        if(lineFlag == 1){
            fprintf(stderr, "Error parsing trace file\n");
            exit(1);
        }
        Process* process = getProcess(&rootProcess, pid);
        ulong filePlace = ftell(fp);
        // if already proceeded
        if(filePlace < process->curr) continue;
        // if just started
        if(process->started == 0) {
            process->started = 1;
            sta->CRP++;
        }
        process->currVpn = vpn;
        process->curr = filePlace;
        // check page
        //if(getPage(pid, vpn) == NULL)
        {
            enQueue(process, IOQueue);
            process->waiting = 1;
            sta->TPI++;
        }
        // if process complete after a normal access, not waiting for IO
        if(filePlace > process->last && !(process->waiting)){
            // endProcess();
            process = NULL;
            sta->CRP--;
        }
        // ticking (this ref action finished)
        sta->RT++;
        if(IOQueue->first != NULL){
            IOQueue->counter++;
        }
        // IO queue action
        if(IOQueue->counter == IOtick){
            Process* newProcess;
            IOQueue->counter = 0;
            newProcess = deQueue(IOQueue);
            newProcess->waiting = 0;
            // Page loaded and get read, additional 1ns spent here
            // loadPage(newProcess->pid, newProcess->currVpn);
            sta->RT++;
            IOQueue->counter++;
            // if the IOtick is 1, there should be a recursive check, unrealistic here
            // If process completes after IO
            if(newProcess->curr == newProcess->last){
                // endProcess(newProcess);
                newProcess = NULL;
                sta->CRP--;
                continue;
            }
            // judge whether to go backward
            if(newProcess->curr < ftell(fp)){
                if(fseek(fp, newProcess->curr, SEEK_SET) == -1){
                    fprintf(stderr, "Error moving file pointer\n");
                    exit(1);
                }
            }
        }


    }
    if(!feof(fp)){
        fprintf(stderr, "Unsupported tracefile format\n");
        exit(1);
    }

}



void loadProcess(FILE* fp, Statistic* sta, Process** root){
    char bufLine[MAX_LINE];
    sta->TMR = 0;
    while(fgets(bufLine, MAX_LINE, fp) != NULL){
        ulong pid, vpn;
        int lineFlag = readPair(&pid, &vpn, bufLine);
        // jump empty line, not adding to TMR
        if(lineFlag == 2) continue;
        sta->TMR++;
        if(lineFlag == 1){
            fprintf(stderr, "Error parsing trace file at line %ld\n", sta->TMR);
            exit(1);
        }
        Process* curProcess;
        if((curProcess = getProcess(root, pid)) == NULL) {
            Process* bufProcess = (Process*)malloc(sizeof(Process));
            if ((curProcess = addProcess(root, bufProcess)) != bufProcess) {
                fprintf(stderr, "Error adding process\n");
                exit(1);
            }
        }
        curProcess->last = ftell(fp);
    }
    if(!feof(fp)){
        fprintf(stderr, "Unsupported tracefile format\n");
        exit(1);
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