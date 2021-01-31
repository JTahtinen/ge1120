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
    , availableMemory(0)
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
    availableMemory = size;
    allocation = (uint8_t*)malloc(allocationSize);
    bool result = allocation;
    return result;
}

unsigned int Memory::reserve(size_t size, void* block)
{
    if (availableMemory < size)
    {
        std::cout << "[ERROR] Could not assign memory - Not enough space!" << std::endl;
    }
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
    
    
    if (reservedMemoryIndices[i + 1] == end)
    {
        for (unsigned int j = i; j < numReservedMemoryIndices - 2; ++j)
        {
            reservedMemoryIndices[j] = reservedMemoryIndices[j + 2];
        }
        numReservedMemoryIndices -= 2;
    }
    else
    {
        reservedMemoryIndices[i] = end;
    }
    
    availableMemory -= size;
    unsigned int handle = numHandles++;
    reservedMemoryInfo[handle] = {startIndex, end};
    return handle;
}

bool Memory::release(unsigned int handle)
{
    bool blockReleased = false;
    if (reservedMemoryInfo.find(handle) == reservedMemoryInfo.end())
    {
        std::cout << "[ERROR] Could not release memory by handle: " << handle << " - Block not found!" << std::endl;
        return false;
    }
    MemoryBlockInfo block = reservedMemoryInfo[handle];
    for (int i = 0; i < numReservedMemoryIndices; ++i)
    {
        if (reservedMemoryIndices[i] == block.start)
        {
            if (reservedMemoryIndices[i + i] != block.end)
            {
                reservedMemoryIndices[i] = block.end;
                blockReleased = true;
                break;
            }
            else
            {
                for (int j = i; j < numReservedMemoryIndices; ++j)
                {
                    if (j + 2 < numReservedMemoryIndices)
                    {
                        reservedMemoryIndices[i] = reservedMemoryIndices[j + 2];
                    }
                    else
                    {
                        numReservedMemoryIndices -= 2;
                        blockReleased = true;
                        break;
                    }
                }
            }
        }
        else if (reservedMemoryIndices[i] == block.end)
        {
            reservedMemoryIndices[i] = block.start;
            blockReleased = true;
            break;
        }
        else if (reservedMemoryIndices[i] > block.end)
        {

            for (int j = numReservedMemoryIndices + 1; j > i + 1; --j)
            {
                reservedMemoryIndices[j] = reservedMemoryIndices[j - 2];
            }
            reservedMemoryIndices[i] = block.start;
            reservedMemoryIndices[i + 1] = block.end;
            numReservedMemoryIndices += 2;
            blockReleased = true;
            break;
        }
        
    }
    if (blockReleased)
    {
        reservedMemoryInfo.erase(handle);
    }
    return blockReleased;
}

void Memory::printState()
{
    std::cout << "Total Allocation: " << allocationSize << 
    " - Available memory: " << availableMemory << 
    " - Num reserved blocks: " << reservedMemoryInfo.size() << std::endl;
    std::cout << "Memory block handles" << std::endl << "-------------" << std::endl; 
    for (std::map<unsigned int,MemoryBlockInfo>::iterator it = reservedMemoryInfo.begin(); it != reservedMemoryInfo.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
    std::cout << "Memory Indices(" << numReservedMemoryIndices << ")" << std::endl << "-------------" << std::endl;
    for (int i = 0; i < numReservedMemoryIndices; ++i)
    {
        std::cout << reservedMemoryIndices[i] << std::endl;
    }
    std::cout << std::endl;
}