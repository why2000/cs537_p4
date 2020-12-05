//
// Created by hanyuan on 12/1/20.
// Team member: Hanyuan Wu, Zhihao Shu

#include "PageAlgorithm.h"
#include <stdlib.h>
#include <search.h>

// private global vars, for twalk usage

typedef struct EntryNode{
    struct EntryNode* next;
    struct PageTableEntry* entry;
} EntryNode;

typedef struct EntryRing{
    struct EntryNode* first;
    struct EntryNode* last;
    ulong size;
} EntryRing;

EntryRing* entryRing;
EntryNode* clock;

/**
* adding a node betwee first and last;
* @param value the node value
* @param queue the ring to work on
*/
void in(PageTableEntry* entry, EntryRing* ring){
    // create
    if(ring->first == NULL){
        if((ring->first = (EntryNode*)malloc(sizeof(EntryNode))) == NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(1);
        }
        ring->first->entry = entry;
        ring->last = ring->first;
        ring->first->next = ring->first;
    }
    else if(ring->first == ring->last){
        if((ring->last = (EntryNode*)malloc(sizeof(EntryNode))) == NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(1);
        }
        ring->last->entry = entry;
        ring->last->next = ring->first;
        ring->first->next = ring->last;

    }
    else{

        ring->last->next = (EntryNode*)malloc(sizeof(EntryNode));
        if(ring->last->next==NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(1);
        }
        ring->last = ring->last->next;
        ring->last->entry = entry;
        ring->last->next = ring->first;
    }
    ring->size++;
}

/**
 * normal dequeue action (and free the dequeued node)
 * @param queue the queue to work on
 * @return the dequeued node's process
 */
PageTableEntry* out(EntryRing* ring){
    if(ring->first == NULL){
        fprintf(stderr, "empty ring delete\n");
        exit(1);
    }
    EntryNode* buf = ring->first;
    PageTableEntry* res = buf->entry;
    if(ring->last == ring->first){
        ring->last = NULL;
        ring->first = NULL;
    }
    else{
        ring->last->next = ring->first->next;
        ring->first = ring->first->next;
    }
    free(buf);
    buf = NULL;
    ring->size--;
    return res;
}

void deleteIn(ulong pid, ulong vpn) {
    if (entryRing->first == NULL) return;
    EntryNode *ptr = entryRing->first;
    if (ptr->entry->pid == pid && ptr->entry->vpn == vpn) {
        out(entryRing);
        return;
    }
    ulong i = 0;
    while (ptr->next != entryRing->first) {
        if(i>=entryRing->size) break;
        if (ptr->next->entry->pid == pid && ptr->next->entry->vpn == vpn) {
            EntryNode *buf = ptr->next;
            ptr->next = ptr->next->next;
            if (buf == entryRing->last)
                entryRing->last = ptr;
            free(buf);
            entryRing->size--;
            return;
        }
        i++;
        ptr = ptr->next;
    }
}

/**
 * load an entry from disk to mem
 * @param root page table root
 * @param pid
 * @param vpn
 * @param sta statistics
 */
void loadEntry(void** root, ulong pid, ulong vpn, Statistic* sta){
    PageTableEntry* entry = (PageTableEntry*)malloc(sizeof(PageTableEntry));
    entry->vpn = vpn;
    entry->pid = pid;
    entry->useTime = sta->RT+1;
    entry->freeBit = 1;
    if(entryRing == NULL){
        entryRing = (EntryRing*)malloc(sizeof(EntryRing));
        entryRing->last = NULL;
        entryRing->first = NULL;
    }
    // already loaded
    if(getEntry(root, pid, vpn) != NULL) return;
    // not full
    if(sta->CMU < sta->TPF){
        in(entry, entryRing);
        if(addEntry(root, entry) == NULL){
            fprintf(stderr, "failed adding entry\n");
            exit(1);
        }
        sta->CMU++;
    }
    else{
        if(clock == NULL) clock = entryRing->first;
        EntryNode* preChosen = NULL;
        while(preChosen == NULL){
            if(clock->next->entry->freeBit == 0){
                preChosen = clock;
            }
            else{
                clock->next->entry->freeBit = 0;
            }
            clock = clock->next;
        }
        EntryNode* buf = preChosen->next;
        preChosen->next = (EntryNode*)malloc(sizeof(EntryNode));
        preChosen->next->entry = entry;
        preChosen->next->next = buf->next;
        deleteEntry(root, buf->entry);
        addEntry(root, entry);

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
        deleteIn(buf->toBeDeleted->pid, buf->toBeDeleted->vpn);
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