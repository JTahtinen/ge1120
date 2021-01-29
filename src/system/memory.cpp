#include "memory.h"
#include <memory>
#include <iostream>

void Memory::sortMemoryBlockIndices()
{

}

Memory::Memory()
    : allocation(nullptr)
    , allocationSize(0)
    , numReservedMemoryIndices(0)
{
}

Memory::Memory(size_t size)
{
    init(size);
}

Memory::~Memory()
{
    free(allocation);
    allocationSize = 0;
}

bool Memory::init(size_t size)
{
    numHandles = 0;
    numReservedMemoryIndices = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = size;
    allocationSize = size;
    allocation = (uint8_t*)malloc(allocationSize);
    bool result = allocation;
    return result;
}

unsigned int Memory::reserve(size_t size, void* block)
{
    if (numReservedMemoryIndices >= 500)
    {
        std::cout << "[ERROR] Could not assign memory - Too many allocation instances!" << std::endl;
    }
    unsigned int i = 0;
    unsigned int startIndex = reservedMemoryIndices[i];
    unsigned int endIndex = reservedMemoryIndices[i + 1];
    if (startIndex + size >= allocationSize)
    {
        std::cout << "[ERROR] Could not assign memory - Allocation limit reached!" << std::endl;
        return -1;
    }
    bool blockFound = false;
    while (!blockFound)
    {   
        unsigned int currentMemBlockSize = endIndex - startIndex; 
        if (startIndex + size <= currentMemBlockSize)
        {
            blockFound = true;
        }
        else
        {
            if (i + 1 == numReservedMemoryIndices)
            {
                std::cout << "[ERROR] Could not reserve memory - no contiguous blocks of appropriate size!" << std::endl;
                return -1;
            }
            i += 2;
            startIndex = reservedMemoryIndices[i];
            endIndex = reservedMemoryIndices[i + 1];
        }
        
    }
    void* memory = allocation + startIndex;
    
    unsigned int end = startIndex + size;
    reservedMemoryIndices[i] = end;
    unsigned int handle = numHandles++;
    reservedMemoryInfo[handle] = {startIndex, end};
    return handle;
}

bool Memory::release(unsigned int handle)
{
    if (reservedMemoryInfo.find(handle) == reservedMemoryInfo.end())
    {
        std::cout << "[ERROR] Could not release memory by handle: " << handle << " - Block not found!" << std::endl;
        return false;
    }
    MemoryBlockInfo block = reservedMemoryInfo[handle];

    // TODO: release code
    return true;
}