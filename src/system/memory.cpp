#include "memory.h"
#include <memory>
#include <iostream>
#include "../globals.h"
#include "../math/math.h"
#include "../graphics/renderer.h"
#include "../defs.h"

Memory::Memory()
    : allocation(nullptr), allocationSize(0), numReservedMemoryIndices(0), availableMemory(0)
{
}

Memory::Memory(size_t size)
{
    init(size);
}

Memory::~Memory()
{
    free(reservedMemoryIndices);
    free(allocation);
    allocationSize = 0;
}

bool Memory::init(size_t size)
{
    reservedMemoryIndices = (unsigned int *)malloc(sizeof(unsigned int) * MAX_MEMORY_INDICES);
    numReservedMemoryIndices = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = size;
    allocationSize = size;
    availableMemory = size;
    allocation = (uint8_t *)malloc(allocationSize);
    bool result = allocation;
    return result;
}

void *Memory::reserve(size_t size)
{
    if (availableMemory < size)
    {
        std::cout << "[ERROR] Could not assign memory - Not enough space!" << std::endl;
        return NULL;
    }
    if (numReservedMemoryIndices >= MAX_MEMORY_INDICES)
    {
        std::cout << "[ERROR] Could not assign memory - Too many allocation instances!" << std::endl;
        return NULL;
    }
    unsigned int i = 0;
    unsigned int startIndex = reservedMemoryIndices[i];
    unsigned int endIndex = reservedMemoryIndices[i + 1];
    if (startIndex + size > allocationSize)
    {
        std::cout << "[ERROR] Could not assign memory - Allocation limit reached!" << std::endl;
        return NULL;
    }
    bool blockFound = false;
    while (!blockFound)
    {
        unsigned int currentMemBlockSize = endIndex - startIndex;
        if (size <= currentMemBlockSize)
        {
            blockFound = true;
        }
        else
        {
            if (i + 1 == numReservedMemoryIndices)
            {
                std::cout << "[ERROR] Could not reserve memory - no contiguous blocks of appropriate size!" << std::endl;
                return NULL;
            }
            i += 2;
            startIndex = reservedMemoryIndices[i];
            endIndex = reservedMemoryIndices[i + 1];
        }
    }
    void *block = allocation + startIndex;

    unsigned int end = startIndex + size;

    if (reservedMemoryIndices[i + 1] == end)
    {
        if (i + 2 != numReservedMemoryIndices)
        {
            for (unsigned int j = i; j < numReservedMemoryIndices - 2; ++j)
            {
                reservedMemoryIndices[j] = reservedMemoryIndices[j + 2];
            }
        }
        else
        {
            reservedMemoryIndices[i] = reservedMemoryIndices[i + 1];
        }
        numReservedMemoryIndices -= 2;
    }
    else
    {
        reservedMemoryIndices[i] = end;
    }

    availableMemory -= size;

    reservedMemoryInfo[block] = {startIndex, end};

#ifdef DEBUG
    if (numReservedMemoryIndices > 1)
        for (int i = 0; i < numReservedMemoryIndices - 1; ++i)
        {
            ASSERT(reservedMemoryIndices[i] != reservedMemoryIndices[i + 1]);
        }
#endif
    return block;
}

bool Memory::release(void *address)
{
    bool blockReleased = false;
    if (reservedMemoryInfo.find(address) == reservedMemoryInfo.end())
    {
        std::cout << "[ERROR] Could not release memory by address: " << address << " - Block not found!" << std::endl;
        return false;
    }
    MemoryBlockInfo block = reservedMemoryInfo[address];
    if (numReservedMemoryIndices == 0)
    {
        reservedMemoryIndices[numReservedMemoryIndices++] = block.start;
        reservedMemoryIndices[numReservedMemoryIndices++] = block.end;
        blockReleased = true;
    }
    else
    {
        for (int i = 0; i < numReservedMemoryIndices; ++i)
        {
            if (reservedMemoryIndices[i] == block.start)
            {
                if (reservedMemoryIndices[i + 1] != block.end)
                {
                    ASSERT(reservedMemoryIndices[i + 1] > block.end);
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
                            reservedMemoryIndices[j] = reservedMemoryIndices[j + 2];
                        }
                        else
                        {
                            numReservedMemoryIndices -= 2;
                            blockReleased = true;
                            break;
                        }
                    }
                }
                break;
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
        if (!blockReleased)
        {
            reservedMemoryIndices[numReservedMemoryIndices++] = block.start;
            reservedMemoryIndices[numReservedMemoryIndices++] = block.end;
            blockReleased = true;
        }
    }
    ASSERT(blockReleased);
    if (blockReleased)
    {
        availableMemory += block.end - block.start;
        try
        {
            reservedMemoryInfo.erase(address);
        }
        catch (int e)
        {
            message("Exception: %d", e);
            __debugbreak();
        }
    }
#ifdef DEBUG
    for (int i = 0; i < numReservedMemoryIndices - 1; ++i)
    {
        if (numReservedMemoryIndices > 1)
        {
            {
                ASSERT(reservedMemoryIndices[i] != reservedMemoryIndices[i + 1]);
            }
        }
    }
#endif
    return blockReleased;
}

void Memory::printState()
{
    std::cout << "Total Allocation: " << allocationSize << " - Available memory: " << availableMemory << " - Num reserved blocks: " << reservedMemoryInfo.size() << std::endl;
    std::cout << "Memory block handles" << std::endl
              << "-------------" << std::endl;
    for (std::map<void*, MemoryBlockInfo>::iterator it = reservedMemoryInfo.begin(); it != reservedMemoryInfo.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
    std::cout << "Memory Indices(" << numReservedMemoryIndices << ")" << std::endl
              << "-------------" << std::endl;
    for (int i = 0; i < numReservedMemoryIndices; ++i)
    {
        std::cout << reservedMemoryIndices[i] << std::endl;
    }
    std::cout << std::endl;
}

void Memory::visualize()
{
    static Vec2 barDim(0.4f, 0.03f);
    static Vec2 barPos(-0.9f, 0.3f);
    Quad barQuad(barPos, barPos + Vec2(0, barDim.y), barPos + barDim, barPos + Vec2(barDim.x, 0));
    g_renderer->submitQuad(barQuad, Vec2(), Vec4(1, 1, 1, 1));
    static float byteSize = barDim.x / allocationSize;
    for (std::map<void*, MemoryBlockInfo>::iterator it = reservedMemoryInfo.begin(); it != reservedMemoryInfo.end(); ++it)
    {
        MemoryBlockInfo block = it->second;
        Vec2 blockStart = Vec2(byteSize * block.start, 0);
        Vec2 blockEnd = Vec2(byteSize * block.end, 0);
        Quad blockQuad(barQuad.point0 + blockStart, barQuad.point1 + blockStart, barQuad.point1 + blockEnd, barQuad.point0 + blockEnd);
        g_renderer->submitQuad(blockQuad, Vec2(), Vec4(1, 0, 0, 1));
    }
}