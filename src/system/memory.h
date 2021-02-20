#pragma once
#include <stdint.h>
#include <map>

#define MAX_MEMORY_INDICES 60000
#define MAX_MEMORY_ALLOCATIONS 60000
struct MemoryBlockInfo
{
    size_t start;
    size_t end;
};

#define KB(val) (val * 1024)
#define MB(val) (KB(val) * 1024)
#define GB(val) (MB(val) * 1024)

class Memory
{
public:

    void* allocation;
    size_t allocationSize;
    size_t availableMemory;

    size_t* reservedMemoryIndices;
    size_t numReservedMemoryIndices;

    std::map<void*, MemoryBlockInfo> reservedMemoryInfo;
    
public:
    Memory();
    Memory(size_t size);
    ~Memory();
    bool init(size_t size);
    void* reserve(size_t size);
    bool release(void* address);
    void printState();
    void visualize();
};
