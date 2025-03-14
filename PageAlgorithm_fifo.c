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

typedef struct EntryQueue{
    struct EntryNode* first;
    struct EntryNode* last;
    ulong size;
} EntryQueue;

EntryQueue* entryQueue;

/**
* normal enqueue action
* @param value the node value
* @param queue the queue to work on
*/
void in(PageTableEntry* entry, EntryQueue* queue){
    // create
    if(queue->first == NULL){
        if((queue->first = (EntryNode*)malloc(sizeof(EntryNode))) == NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(1);
        }
        queue->first->entry = entry;
        queue->last = queue->first;
    }
    else{
        // expand last
        queue->last->next = (EntryNode*)malloc(sizeof(EntryNode));
        if(queue->last->next==NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(1);
        }
        queue->last = queue->last->next;
        queue->last->entry = entry;
        queue->last->next = NULL;
    }
    queue->size++;
}

/**
 * normal dequeue action (and free the dequeued node)
 * @param queue the queue to work on
 * @return the dequeued node's process
 */
PageTableEntry* out(EntryQueue* queue){
    if(queue->first == NULL){
        fprintf(stderr, "Attemp to dequeue an empty queue\n");
        exit(1);
    }
    EntryNode* buf = queue->first;
    PageTableEntry* res = buf->entry;
    if(queue->last == queue->first){
        queue->last = NULL;
        queue->first = NULL;
    }
    else{
        queue->first = queue->first->next;
    }
    free(buf);
    buf = NULL;
    queue->size--;
    return res;
}

/**
 * delete from inside the queue
 * @param pid
 * @param vpn
 */
void deleteIn(ulong pid, ulong vpn) {
    if (entryQueue->first == NULL) return;
    EntryNode *ptr = entryQueue->first;
    if (ptr->entry->pid == pid && ptr->entry->vpn == vpn) {
        out(entryQueue);
        return;
    }
    if (ptr->next == NULL) return;
    while (ptr->next != NULL) {
        if (ptr->next->entry->pid == pid && ptr->next->entry->vpn == vpn) {
            EntryNode *buf = ptr->next;
            ptr->next = ptr->next->next;
            if (buf == entryQueue->last) entryQueue->last = ptr;
            free(buf);
            entryQueue->size--;
            return;
        }
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
    if(entryQueue == NULL){
        entryQueue = (EntryQueue*)malloc(sizeof(EntryQueue));
        entryQueue->last = NULL;
        entryQueue->first = NULL;
    }
    PageTableEntry* entry = (PageTableEntry*)malloc(sizeof(PageTableEntry));
    entry->vpn = vpn;
    entry->pid = pid;
    entry->useTime = sta->RT+1;
    // already loaded
    if(getEntry(root, pid, vpn) != NULL) return;
    // not full
    if(sta->CMU < sta->TPF){
        if(addEntry(root, entry) == NULL){
            fprintf(stderr, "failed adding entry\n");
            exit(1);
        }
        in(entry, entryQueue);
        sta->CMU++;
    }
    else{
        PageTableEntry* buf = out(entryQueue);
        deleteEntry(root, buf);
        if(addEntry(root, entry) == NULL){
            fprintf(stderr, "failed adding entry\n");
            exit(1);
        }
        in(entry, entryQueue);
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




