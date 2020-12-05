//
// Created by hanyuan on 12/1/20.
// Team member: Hanyuan Wu, Zhihao Shu

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include "traceReader.h"

//void printPageTable(const void* nodeP, const VISIT which, int level){
//    PageTableEntry* node = *(PageTableEntry**) nodeP;
//    if(which == postorder||which == leaf){
//        printf("pid:%ld, vpn:%ld\n", node->pid, node->vpn);
//    }
//}

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
    sta->TPF = realMemSize/pageSize;
    void* rootProcess;
    loadProcess(fp, sta, &rootProcess);
    void* rootEntry;
    fseek(fp, 0, SEEK_SET);
    // value is pid
    Queue* IOQueue;
    if((IOQueue = (Queue*)malloc(sizeof(Queue))) == NULL){
        fprintf(stderr, "Unable to malloc\n");
        exit(1);
    }
    IOQueue->first = NULL;
    IOQueue->last = NULL;
    while(IOQueue->numIO != 0 || sta->CRP>0) {
        int IOhold = 0;
        if (!feof(fp) && sta->CRP > 0) {
            // get data
            ulong pid, vpn;
            fgets(bufLine, MAX_LINE, fp);
            int lineFlag = readPair(&pid, &vpn, bufLine);
            if (lineFlag == 2) continue;
            if (lineFlag == 1) {
                fprintf(stderr, "Error parsing trace file\n");
                exit(1);
            }
            ulong filePlace = ftell(fp);
            Process *process;
            // check ended
            if((process = getProcess(&rootProcess, pid)) == NULL) continue;
            // if already proceeded
            if (filePlace <= process->curr || process->waiting == 1 || process->ended) continue;
            // if just started
            if (process->started == 0) {
                process->started = 1;
            }

            process->currVpn = vpn;
            process->curr = filePlace;
            PageTableEntry* entry;
            // check page
            if ((entry = getEntry(&rootEntry, pid, vpn)) == NULL) {
                if(IOQueue->numIO == 0)
                    IOhold = 1;
                enQueue(process, IOQueue);
                process->waiting = 1;
                sta->CRP--;
                sta->TPI++;
                //printf("PID:%ld, VPN:%ld, tick:%ld, TMU:%ld, CMU:%ld\n", pid, vpn, sta->RT+1, sta->TMU, sta->CMU);
            }else{
                entry->useTime = sta->RT+1;
                entry->freeBit = 1;
            }
            // if process complete after a normal access, not waiting for IO
            if (filePlace >= process->last && !(process->waiting)) {
                // decrease CRP and CMUs
                process->ended = 1;
                endProcess(&rootEntry, &rootProcess, process, sta);
                process = NULL;
            }
            sta->RT++;
            sta->TMU+=sta->CMU;
            sta->TRP+=sta->CRP;

            if(IOQueue->numIO != 0){
                IOQueue->counter++;
                if(IOhold == 1){
                    IOQueue->counter--;
                    IOhold = 0;
                }
            }
        }
        else{
            if(IOQueue->numIO != 0) {
                ulong period = IOtick - IOQueue->counter;
                IOQueue->counter = IOtick;
                sta->RT += period;
                sta->TMU += sta->CMU * period;
                sta->TRP += sta->CRP * period;
            }
        }

        // IO queue action
        if(IOQueue->counter == IOtick && IOQueue->numIO != 0){
            Process* newProcess;
            IOQueue->counter = 0;
            if((newProcess = deQueue(IOQueue))==NULL)continue;
            newProcess->waiting = 0;
            sta->CRP++;

            // Page loaded and get read, additional 1ns spent here
            // change CMU inside
            loadEntry(&rootEntry, newProcess->pid, newProcess->currVpn, sta);
//            if(newProcess->pid == 9&&newProcess->currVpn == 2){
//                twalk(rootEntry, printPageTable);
//            }
            sta->RT++;
            sta->TMU+=sta->CMU;
            sta->TRP+=sta->CRP;
            if(IOQueue->numIO != 0) {
                IOQueue->counter++;
            }
            // if the IOtick is 1, there should be a recursive check, unrealistic here
            // If process completes after IO
            if(newProcess->curr == newProcess->last){
                // decrease CRP and CMUs
                newProcess->ended = 1;
                endProcess(&rootEntry, &rootProcess, newProcess, sta);
                newProcess = NULL;
                continue;
            }
            //printf("PID:%ld, VPN:%ld, tick:%ld, TMU:%ld, CMU:%ld\n", newProcess->pid, newProcess->currVpn, sta->RT, sta->TMU, sta->CMU);
            // judge whether to go backward
            if(newProcess->curr < (ulong)ftell(fp)){
                if(fseek(fp, newProcess->curr, SEEK_SET) == -1){
                    fprintf(stderr, "Error moving file pointer\n");
                    exit(1);
                }
            }
        }

    }

    outputStatistic(tracefile, sta);

}

void outputStatistic(const char* const tracefile, const Statistic* const sta){
    printf("Tracefile name: %s\n", tracefile);
    printf("AMU: %f\n", 1.0*(sta->TMU)/(1.0*(sta->TPF)*(sta->RT)));
    printf("ARP: %f\n", 1.0*(sta->TRP)/(sta->RT));
    printf("TMR: %ld\n", sta->TMR);
    printf("TPI: %ld\n", sta->TPI);
    printf("Running Time (ticks): %ld\n", sta->RT);
}

void loadProcess(FILE* fp, Statistic* sta, void** root){
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
            bufProcess->pid = pid;
            if ((curProcess = addProcess(root, bufProcess)) != bufProcess) {
                fprintf(stderr, "Error adding process\n");
                exit(1);
            }
            sta->CRP++;
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