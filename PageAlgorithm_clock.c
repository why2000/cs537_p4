//
// Created by hanyuan on 12/5/20.
//

#include "PageAlgorithm.h"
#include <stdlib.h>
#include <search.h>

// private global vars, for twalk usage
int saved = 0;
int idx;
int pageNum;
PageTableEntry* available;

// a clock on the tree :P
void findAvailable(const void* nodeP, const VISIT which, int level){
    PageTableEntry* node = *(PageTableEntry**) nodeP;
    if(which ==  preorder||which == leaf){
        if(idx>=saved && available == NULL){
            if(node->freeBit == 0){
                available = node;
                saved = idx;
            }
            else{
                node->freeBit = 0;
            }
            if(++idx >= pageNum) idx = 0;
        }
    }
}


void loadEntry(void** root, ulong pid, ulong vpn, Statistic* sta){
    PageTableEntry* entry = (PageTableEntry*)malloc(sizeof(PageTableEntry));
    entry->vpn = vpn;
    entry->pid = pid;
    entry->useTime = sta->RT+1;
    pageNum = sta->CMU;
    // already loaded
    if(getEntry(root, pid, vpn) != NULL) return;
    // not full
    if(sta->CMU < sta->TMR){
        if(addEntry(root, entry) == NULL){
            fprintf(stderr, "failed adding entry\n");
            exit(1);
        }
        sta->CMU++;
    }
    else{
        idx = 0;
        if(saved>pageNum) saved = 0;
        available = NULL;
        twalk(root, findAvailable);
        deleteEntry(root, available);
        available = NULL;
        if(addEntry(root, entry) == NULL){
            fprintf(stderr, "failed adding entry\n");
            exit(1);
        }
        // CMU not changed
    }

}

// process end actions
static ulong deletePid;
struct DeleteList{
    PageTableEntry* toBeDeleted;
    struct DeleteList* next;
} *deleteList;

static void deleteAction(const void* nodeP, const VISIT which, int level){
    PageTableEntry* node = *(PageTableEntry**) nodeP;
    if(which ==  preorder||which == leaf){
        if(node->pid == deletePid){
            deleteList->toBeDeleted = (PageTableEntry*)malloc(sizeof(PageTableEntry));
            deleteList->toBeDeleted->pid = node->pid;
            deleteList->toBeDeleted->vpn = node->vpn;
            deleteList->next = (struct DeleteList*)malloc(sizeof(struct DeleteList));
            deleteList = deleteList->next;
            deleteList->next = NULL;
        }
    }
}

void endProcess(void** PTRoot, void** processRoot, Process* process, Statistic* sta) {
    deletePid = process->pid;
    deleteList = (struct DeleteList*)malloc(sizeof(struct DeleteList));
    struct DeleteList* head = deleteList;
    twalk(*PTRoot, deleteAction);
    struct DeleteList* ptr = head;
    while(ptr->next != NULL){
        struct DeleteList* buf = ptr;
        ptr = ptr->next;
        deleteEntry(PTRoot, buf->toBeDeleted);
        sta->CMU--;
    }
    while(head->next != NULL){
        struct DeleteList* buf = head;
        head = head->next;
        free(buf);
        buf = NULL;
    }
    deleteProcess(processRoot, process);
    sta->CRP--;
}