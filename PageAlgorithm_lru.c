//
// Created by hanyuan on 12/5/20.
//

#include "PageAlgorithm.h"
#include <stdlib.h>
#include <search.h>

// private global vars, for twalk usage
static PageTableEntry* least;

static void findLeast(const void* nodeP, const VISIT which, int level){
    PageTableEntry* node = *(PageTableEntry**) nodeP;
    if(which ==  preorder||which == leaf){
        if(least == NULL) least = node;
        else{
            if(node->useTime<least->useTime) least = node;
        }
    }
}


void loadEntry(void** root, ulong pid, ulong vpn, Statistic* sta){

    PageTableEntry* entry = (PageTableEntry*)malloc(sizeof(PageTableEntry));
    entry->vpn = vpn;
    entry->pid = pid;
    entry->useTime = sta->RT+1;

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
        twalk(root, findLeast);
        deleteEntry(root, least);
        least = NULL;
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

static void deleteAction(const void* nodeP, const VISIT which, int foo){
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