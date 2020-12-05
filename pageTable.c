//
// Created by hanyuan on 12/1/20.
//

#include "pageTable.h"
#include <search.h>


/**
 * Get an entry from the tree with root
 * @param root of the tree
 * @param pid of the target entry
 * @param vpn of the target entry
 * @return the entry found (NULL if not found)
 */
PageTableEntry* getEntry(void** root, ulong pid, ulong vpn){
    struct PageTableEntry* res;
    void* buf;
    PageTableEntry keyEntry;
    keyEntry.pid = pid;
    keyEntry.vpn = vpn;
    if((buf = tfind(&keyEntry, root, entryCompare)) == NULL){
        return NULL;
    }
    res = (PageTableEntry*) buf;
    return res;
}

/**
 * Add an entry to the tree from root and return it
 * if it already exists then return the one in the tree
 * @param root the tree's root
 * @param entry to be added
 * @return the added entry
 */
PageTableEntry* addEntry(void** root, PageTableEntry* entry){
    ulong pid = entry->pid;
    ulong vpn = entry->vpn;
    void* buf;
    PageTableEntry* res;
    if((res = getEntry(root, pid, vpn)) != NULL){
        return res;
    }
    else{
        if((buf = tsearch(entry, root, entryCompare)) == NULL){
            fprintf(stderr, "Unable to add node\n");
            exit(1);
        }
        res = (PageTableEntry*) buf;
        return res;
    }
}

/**
 * delete an entry from the tree from root and return it
 * if it is not in the tree, return NULL
 * @param root the tree's root
 * @param entry to be deleted
 * @return the deleted entry (NULL if not found)
 */
PageTableEntry* deleteEntry(void** root, PageTableEntry* entry){
    ulong pid = entry->pid;
    ulong vpn = entry->vpn;
    void* buf;
    PageTableEntry* res;
    if((res = getEntry(root, pid, vpn)) != NULL){
        return res;
    }
    else{
        if((buf = tdelete(entry, root, entryCompare)) == NULL){
            fprintf(stderr, "Unable to delete node\n");
            exit(1);
        }
        res = (PageTableEntry*) buf;
        return res;
    }
}




int entryCompare(const void* a, const void* b){
    PageTableEntry* pA = (PageTableEntry*)a;
    PageTableEntry* pB = (PageTableEntry*)b;
    if(pA->pid == pB->pid && pA->vpn == pB->vpn) return 0;
    // not that balanced, but does not matter (as we only need to search)
    else if(pA->pid + pA->vpn > pB->pid + pB->vpn) return 1;
    else return -1;
}

