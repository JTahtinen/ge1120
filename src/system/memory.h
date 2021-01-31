#pragma once
#include <stdint.h>
#include <map>

#define MAX_MEMORY_INDICES 500

struct MemoryBlockInfo
{
    unsigned int start;
    unsigned int end;
};

class Memory
{
public:

    uint8_t* allocation;
    size_t allocationSize;
    size_t availableMemory;

    size_t reservedMemoryIndices[MAX_MEMORY_INDICES];
    unsigned int numReservedMemoryIndices;

    std::map<unsigned int, MemoryBlockInfo> reservedMemoryInfo;
    unsigned int numHandles;
    
public:
    Memory();
    Memory(size_t size);
    ~Memory();
    bool init(size_t size);
    unsigned int reserve(size_t size, void* block);
    bool release(unsigned int handle);
    void printState();
private:
    void sortMemoryBlockIndices();
};