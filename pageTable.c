//
// Created by hanyuan on 12/1/20.
//

#include "pageTable.h"
#include <search.h>
#include <stdio.h>

//void printPageTable2(const void* nodeP, const VISIT which, int level){
//    PageTableEntry* node = *(PageTableEntry**) nodeP;
//    if(which == postorder||which == leaf){
//        printf("pid:%ld, vpn:%ld\n", node->pid, node->vpn);
//    }
//}


/**
 * Get an entry from the tree with root
 * @param root of the tree
 * @param pid of the target entry
 * @param vpn of the target entry
 * @return the entry found (NULL if not found)
 */
PageTableEntry* getEntry(void** root, ulong pid, ulong vpn){
    struct PageTableEntry* res;
    void* buf = NULL;
    PageTableEntry keyEntry;
    keyEntry.pid = pid;
    keyEntry.vpn = vpn;
//    if(pid == 9 && vpn == 2){
//        twalk(*root, printPageTable2);
//    }
    buf = tfind(&keyEntry, root, entryCompare);
    if(buf == NULL){
        return NULL;
    }
    res = *(PageTableEntry**) buf;
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
    void* buf = NULL;
    PageTableEntry* res;
    if((buf = tsearch(entry, root, entryCompare)) == NULL){
        fprintf(stderr, "Unable to add node\n");
        exit(1);
    }
    res = *(PageTableEntry**) buf;
    return res;

}

/**
 * delete an entry from the tree from root and return it
 * if it is not in the tree, return NULL
 * @param root the tree's root
 * @param entry to be deleted
 * @return the deleted entry (NULL if not found)
 */
void deleteEntry(void** root, PageTableEntry* entry){
    ulong pid = entry->pid;
    ulong vpn = entry->vpn;
    void* buf;
    PageTableEntry* res;
    if(getEntry(root, pid, vpn) == NULL){
        return;
    }
    else{
        if(tdelete(entry, root, entryCompare) == NULL){
            fprintf(stderr, "Unable to delete node\n");
            exit(1);
        }
    }
}


static ulong hash(ulong pid, ulong vpn){
    const int shift = 22;
    return (pid<<shift)+vpn;
}


int entryCompare(const void* a, const void* b){
    PageTableEntry* pA = (PageTableEntry*)a;
    PageTableEntry* pB = (PageTableEntry*)b;
    ulong hashValueA = hash(pA->pid, pA->vpn);
    ulong hashValueB = hash(pB->pid, pB->vpn);
    if(hashValueA > hashValueB) return 1;
    // not that balanced, but does not matter (as we only need to search)
    else if(hashValueA < hashValueB) return -1;
    else return 0;
}

