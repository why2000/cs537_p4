# Using prams given in csa-makefile
CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXE = 537make
SCAN_BUILD_DIR = scan-build-out

537pfsim: 537pfsim-clock 537pfsim-lru 537pfsim-fifo


537pfsim-clock: 537pfsim.o PageAlgorithm_clock.o pageTable.o traceReader.o process.o
	$(CC) -o 537pfsim-clock 537pfsim.o traceReader.o process.o pageTable.o PageAlgorithm_clock.o

537pfsim-lru: 537pfsim.o PageAlgorithm_lru.o pageTable.o traceReader.o process.o
	$(CC) -o 537pfsim-lru 537pfsim.o PageAlgorithm_lru.o pageTable.o traceReader.o process.o

537pfsim-fifo: 537pfsim.o PageAlgorithm_fifo.o pageTable.o traceReader.o process.o
	$(CC) -o 537pfsim-fifo 537pfsim.o PageAlgorithm_fifo.o pageTable.o traceReader.o process.o

PageAlgorithm_clock.o: PageAlgorithm_clock.c PageAlgorithm.h process.h pageTable.h
	$(CC) $(WARNING_FLAGS) -c PageAlgorithm_clock.c

PageAlgorithm_lru.o: PageAlgorithm_lru.c PageAlgorithm.h process.h pageTable.h
	$(CC) $(WARNING_FLAGS) -c PageAlgorithm_lru.c

PageAlgorithm_fifo.o: PageAlgorithm_fifo.c PageAlgorithm.h process.h pageTable.h
	$(CC) $(WARNING_FLAGS) -c PageAlgorithm_fifo.c

pageTable.o: pageTable.c pageTable.h
	$(CC) $(WARNING_FLAGS) -c pageTable.c

traceReader.o: traceReader.c traceReader.h PageAlgorithm.h process.h 537pfsim.h
	$(CC) $(WARNING_FLAGS) -c traceReader.c

process.o: process.c process.h pageTable.h
	$(CC) $(WARNING_FLAGS) -c process.c

537pfsim.o: 537pfsim.c 537pfsim.h traceReader.h
	$(CC) $(WARNING_FLAGS) -c 537pfsim.c

clean:
	rm -f $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)


scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make


scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html
