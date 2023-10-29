// memory_manager.h

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <unistd.h>
#include <list>
#include <string>
#include <string.h>
using namespace std;

enum AllocStrat {First, Best};

class MemoryManager {
public:
    MemoryManager(AllocStrat strategy);
    ~MemoryManager();

    // Function to allocate memory chunk of specified size
    void* alloc(size_t chunk_size);

    // Function to deallocate a previously allocated memory chunk
    void dealloc(void* chunk);

    // Prints the allocated list chunk by chunk
    void printAllocated();

    // Print the free list chunk by chunk
    void printFree();

    // Print internal/external fragmentation amount in memory
    void printIntFrag();
    void printExtFrag();

private:
    struct Allocation {
        size_t size;
        size_t used_size;
        void* space;
    };

    list<Allocation*> allocatedChunks; // List of pointers to occupied chunks
    list<Allocation*> freeChunks;     // List of pointers to free chunks

    void* heapFrontier;
    AllocStrat selectedStrategy;
};

#endif // MEMORY_MANAGER_H
