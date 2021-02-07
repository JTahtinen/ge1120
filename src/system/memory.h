#pragma once
#include <stdint.h>
#include <map>

#define MAX_MEMORY_INDICES 60000
#define MAX_MEMORY_ALLOCATIONS 60000
struct MemoryBlockInfo
{
    unsigned int start;
    unsigned int end;
};

#define KB(val) (val * 1024)
#define MB(val) (KB(val) * 1024)
#define GB(val) (MB(val) * 1024)

class Memory
{
public:

    uint8_t* allocation;
    size_t allocationSize;
    size_t availableMemory;

    unsigned int* reservedMemoryIndices;
    unsigned int numReservedMemoryIndices;

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