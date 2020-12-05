
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "537pfsim.h"
#include "traceReader.h"

/**
 * private method, just to print usage...
 * @return always return 1
 */
int pUsage(){
    printf("Usage: 537pfsim [-p <pageSize>] [-m <realMemSize>] <tracefile>");
    return 1;
}

int main(int argc, char** argv) {

    const int defaultPageSize = 4096;
    const int defaultRealMemSize = 1;
    char tracefile[MAX_LINE];
    tracefile[0] = '\0';
    ulong pageSize = 0;
    ulong realMemSize = 0;
    const int MAX_ARG = 6;
    if(argc > MAX_ARG || argc % 2)
        return pUsage();
    // only few args, no need to use getopt
    for(int i = 1; i < argc; i++){
        if((strncmp("-p", argv[i], MAX_LINE) == 0) && pageSize == 0){
            char* endBuf;
            if(++i >= argc || (pageSize = strtoul(argv[i], &endBuf, 10)) == 0)
                return pUsage();
            if(((uint)pageSize & (uint)pageSize-1) != 0){
                fprintf(stderr, "Page size must be 2^n, current size %ld not allowed\n", pageSize);
                exit(1);
            }
        }
        else if((strncmp("-m", argv[i], MAX_LINE) == 0) && realMemSize == 0){
            char* endBuf;
            if(++i >= argc || (realMemSize = strtoul(argv[i], &endBuf, 10)) == 0)
                return pUsage();
        }
        else{
            snprintf(tracefile, MAX_LINE, "%s", argv[i]);
        }
    }
    if(tracefile[0] == '\0')
        return pUsage();
    // set params
    pageSize = (pageSize == 0? defaultPageSize: pageSize);
    realMemSize = (realMemSize == 0? defaultRealMemSize: realMemSize);
    realMemSize *= (1<<20);
    if(pageSize > realMemSize){
        fprintf(stderr, "Error: Page size %ld should not be larger than real memory size %ld\n",
                pageSize, realMemSize);
        exit(1);
    }
    // start tracing
    parseTrace(tracefile, pageSize, realMemSize);
    return 0;

}


