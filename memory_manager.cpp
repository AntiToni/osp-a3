#include "memory_manager.h"

int main(int argc, char* argv[]) {
    string currentExecName = argv[0]; // Name of the current exec program
    string filePath = "";

    AllocStrat selectedStrategy;

    // Error checking
    if (currentExecName.substr(currentExecName.size() - 8) == "firstfit") {
        selectedStrategy = AllocStrat::First;
    }
    else if (currentExecName.substr(currentExecName.size() - 7) == "bestfit") {
        selectedStrategy = AllocStrat::Best;
    }
    else {
        cout << "Invalid file name, must be one of: \"firstfit\" or \"bestfit\"." << endl;
        return EXIT_FAILURE;
    }

    if (argc != 2) {
        cout << "Invalid arguments, must be only one datafile argument." << endl;
        return EXIT_FAILURE;
    }
    
    filePath = argv[1];

    // Create MemoryManager
    MemoryManager manager = MemoryManager(selectedStrategy);

    // Create local chunk list
    list<void*> chunkList;

    // Read input file and perform memory functions
    ifstream file(filePath);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.substr(0, 6) == "alloc:") {
                int chunk_size = stoi(line.substr(7));
                void* chunk = manager.alloc(chunk_size);
                chunkList.push_back(chunk);
            }
            else if (line.substr(0, 7) == "dealloc") {
                // Check that the chunkList isn't empty
                if (!chunkList.empty()) {
                    void* latestAlloc = chunkList.back();
                    chunkList.pop_back();
                    manager.dealloc(latestAlloc);
                }
            }
        }
        file.close();
    } else {
        cout << "The file '" << filePath << "' does not exist." << endl;
        return EXIT_FAILURE;
    }

    // Print both lists
    manager.printAllocated();
    manager.printFree();
}

MemoryManager::MemoryManager(AllocStrat strategy) {
    allocatedChunks = list<Allocation*>();
    freeChunks = list<Allocation*>();

    heapFrontier = sbrk(0);
    selectedStrategy = strategy;
}

MemoryManager::~MemoryManager() {
    // Deallocate memory for each chunk in the allocated list
    for (auto allocation : allocatedChunks) {
        delete allocation; // Release the Allocation structure
    }

    // Clean up free list
    for (auto allocation : freeChunks) {
        delete allocation; // Release the Allocation structure
    }

    // Clear both lists
    freeChunks.clear();
    allocatedChunks.clear();

    // Move the program break back to original position
    if (brk(heapFrontier) != 0) {
        // Handle error if brk fails
        int err = errno; // Get the error code

        if (err == ENOMEM) {
            cerr << "brk failed: Out of memory" << endl;
        } else if (err == EINVAL) {
            cerr << "brk failed: Invalid argument" << endl;
        } else {
            cerr << "brk failed with error code " << err << ": " << strerror(err) << endl;
        }
    }
}

void * MemoryManager::alloc(size_t chunk_size) {
    // Error checking
    if (chunk_size > 512 || chunk_size < 1) {
        return NULL;
    }

    Allocation* foundChunk = nullptr;

    if (selectedStrategy == AllocStrat::First) {
        // Find first chunk which is >= chunk_size
        for (auto it = freeChunks.begin(); it != freeChunks.end(); ++it) {
            Allocation* allocation = *it;
            if (allocation->size >= chunk_size) {
                // Found a free chunk with sufficient size
                freeChunks.erase(it);  // Remove the chunk from the list
                foundChunk = allocation;
                break;
            }
        }
    }
    else if (selectedStrategy == AllocStrat::Best) {
        // Check for min sized chunk at least as large as chunk_size
        auto minSizeChunk = std::min_element(freeChunks.begin(), freeChunks.end(), 
            // Lambda to compare values
            [chunk_size](const Allocation* a, const Allocation* b) {
                return a->size < b->size && a->size >= chunk_size;
            });

        // If a chunk is found that matches, erase from list
        if (minSizeChunk != freeChunks.end()) {
            foundChunk = *minSizeChunk;
            freeChunks.erase(minSizeChunk);
        }
    }

    // If chunk found, add to allocatedChunks list
    if (foundChunk != nullptr) {
        foundChunk->used_size = chunk_size;
        allocatedChunks.push_back(foundChunk);

        return foundChunk;
    }
    // Otherwise, create new chunk with smallest valid size, and add to allocatedChunks list
    else {
        // Determine smallest valid chunk size
        const int sizes[] = {32, 64, 128, 256, 512};
        int smallest = 512;  // Start with the largest value as the default.
        for (int size : sizes) {
            if (static_cast<size_t>(size) >= chunk_size && size < smallest) {
                smallest = size;
            }
        }

        void* allocatedSpace = sbrk(static_cast<intptr_t>(smallest));

        if (allocatedSpace == (void*)-1) {
            // Handle allocation error
            cerr << "Failed to allocate space using sbrk" << endl;
            return NULL;
        }

        Allocation* newAllocation = new Allocation;
        newAllocation->size = smallest;
        newAllocation->used_size = chunk_size;
        newAllocation->space = allocatedSpace;
        allocatedChunks.push_back(newAllocation);

        return newAllocation;
    }
}

void MemoryManager::dealloc(void* chunk) {
    // Search for the chunk in the allocatedChunks list
    auto it = find(allocatedChunks.begin(), allocatedChunks.end(), chunk);

    if (it != allocatedChunks.end()) {
        // Move allocation to free list and reset used_size
        Allocation* allocation = *it;
        freeChunks.erase(it); 
        allocation->used_size = 0;
        freeChunks.push_back(allocation);
    } else {
        // The allocation was not found
        cerr << "Allocation not found. Aborting program." << endl;
        terminate();
    }
}

void MemoryManager::printAllocated() {
    cout << "Chunks in the Allocated List:" << endl;
    for (const Allocation* allocation : allocatedChunks) {
        cout << "Address: " << allocation->space << ", Total Size: " << allocation->size << ", Used Size: " << allocation->used_size << endl;
    }
}

void MemoryManager::printFree() {
    cout << "Chunks in the Free List:" << endl;
    for (const Allocation* allocation : freeChunks) {
        cout << "Address: " << allocation->space << ", Total Size: " << allocation->size << endl;
    }
}