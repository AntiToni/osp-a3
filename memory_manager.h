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
    MemoryManager();

    // Function to allocate memory chunk of specified size
    void* alloc(size_t chunk_size);

    // Function to deallocate a previously allocated memory chunk
    void dealloc(void* chunk);

    // Function to set the allocation strategy (not implemented in this class)
    // Implement this in your own code.

private:
    struct Allocation {
        size_t size;
        size_t used_size;
        void* space;
    };

    list<Allocation*> allocatedChunks; // List of pointers to occupied chunks
    list<Allocation*> freeChunks;     // List of pointers to free chunks
};

#endif // MEMORY_MANAGER_H
