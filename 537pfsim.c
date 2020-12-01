
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "537pfsim.h"

int pUsage(){
    printf("Usage: 537pfsim [-p <pageSize>] [-m <realMemSize>] <tracefile>");
    return 1;
}

int main(int argc, char** argv) {

    const int defaultPageSize = 4096;
    const int defaultRealMemSize = 1;
    char tracefile[MAX_LINE];
    tracefile[0] = '\0';
    long pageSize = 0;
    long realMemSize = 0;
    const int MAX_ARG = 6;
    if(argc > MAX_ARG || argc % 2)
        return pUsage();
    // only few args, no need to use getopt

    for(int i = 1; i < argc; i++){
        if((strncmp("-p", argv[i], MAX_LINE) == 0) && pageSize == 0){
            char* endBuf;
            if(++i >= argc || (pageSize = strtol(argv[i], &endBuf, 10)) <= 0)
                return pUsage();
            if(((ulong)pageSize & (ulong)pageSize-1) != 0){
                fprintf(stderr, "Page size must be 2^n, current size %ld not allowed\n", pageSize);
                exit(1);
            }
        }
        else if((strncmp("-m", argv[i], MAX_LINE) == 0) && realMemSize == 0){
            char* endBuf;
            if(++i >= argc || (realMemSize = strtol(argv[i], &endBuf, 10)) <= 0)
                return pUsage();
        }
        else{
            snprintf(tracefile, MAX_LINE, "%s", argv[i]);
        }
    }
    if(tracefile[0] == '\0')
        return pUsage();
    pageSize = (pageSize == 0? defaultPageSize: pageSize);
    realMemSize = (realMemSize == 0? defaultRealMemSize: realMemSize);
    printf("%ld\t%ld\t%s", pageSize, realMemSize, tracefile);
    return 0;

}


