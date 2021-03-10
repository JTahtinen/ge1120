#include "memory.h"
#include <memory>
#include "../globals.h"
#include "../math/math.h"
#include "../graphics/renderer.h"
#include "../defs.h"
//#include <string>
#include "../util/string.h"
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
    reservedMemoryIndices = (size_t *)malloc(sizeof(size_t) * MAX_MEMORY_INDICES);
    numReservedMemoryIndices = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = 0;
    reservedMemoryIndices[numReservedMemoryIndices++] = size;
    allocationSize = size;
    availableMemory = size;
    allocation = (void*)malloc(allocationSize);
    bool result = allocation;
    return result;
}

void *Memory::reserve(size_t size)
{
    if (size ==  0)
    {
        return NULL;
    }
    if (availableMemory < size)
    {
        err("Could not assign memory - Not enough space!\n");
        return NULL;
    }
    if (numReservedMemoryIndices >= MAX_MEMORY_INDICES)
    {
        err("Could not assign memory - Too many allocation instances!\n");
        return NULL;
    }
    size_t i = 0;
    size_t startIndex = reservedMemoryIndices[i];
    size_t endIndex = reservedMemoryIndices[i + 1];
    if (startIndex + size > allocationSize)
    {
        err("Could not assign memory - Allocation limit reached!\n");
        return NULL;
    }
    bool blockFound = false;
    while (!blockFound)
    {
        size_t currentMemBlockSize = endIndex - startIndex;
        if (size <= currentMemBlockSize)
        {
            blockFound = true;
        }
        else
        {
            if (i + 1 == numReservedMemoryIndices)
            {
                err("Could not reserve memory - no contiguous blocks of appropriate size!\n");
                return NULL;
            }
            i += 2;
            startIndex = reservedMemoryIndices[i];
            endIndex = reservedMemoryIndices[i + 1];
        }
    }
    uint8_t *block = (uint8_t*)allocation + startIndex;

    size_t end = startIndex + size;

    if (reservedMemoryIndices[i + 1] == end)
    {
        if (i + 2 != numReservedMemoryIndices)
        {
            memmove(&reservedMemoryIndices[i], &reservedMemoryIndices[i + 2],
                    sizeof(size_t) * (numReservedMemoryIndices - i - 2));
            /*
            for (size_t j = i; j < numReservedMemoryIndices - 2; ++j)
            {
                reservedMemoryIndices[j] = reservedMemoryIndices[j + 2];
            }
            */
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
    ASSERT(block);
    return block;
}

bool Memory::release(void *address)
{
    bool blockReleased = false;
    if (reservedMemoryInfo.find(address) == reservedMemoryInfo.end())
    {
        err("Could not release memory by address: %d - Block not found!\n", address);
        return false;
    }
    MemoryBlockInfo block = reservedMemoryInfo[address];
//    memset((uint8_t*)allocation + block.start, 0, block.end - block.start);
    if (numReservedMemoryIndices == 0)
    {
        reservedMemoryIndices[numReservedMemoryIndices++] = block.start;
        reservedMemoryIndices[numReservedMemoryIndices++] = block.end;
        blockReleased = true;
    }
    else
    {
        for (size_t i = 0; i < numReservedMemoryIndices; ++i)
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
                    
                    memmove(&reservedMemoryIndices[i], &reservedMemoryIndices[i + 2],
                            sizeof(size_t) * (numReservedMemoryIndices - i - 2));
                    numReservedMemoryIndices -= 2;
                    blockReleased = true;
            
/*
                    for (size_t j = i; j < numReservedMemoryIndices; ++j)
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
                 */
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
                memmove(&reservedMemoryIndices[i + 2], &reservedMemoryIndices[i],
                        sizeof(size_t) * (numReservedMemoryIndices - i));
                       
/*
                for (size_t j = numReservedMemoryIndices + 1; j > i + 1; --j)
                {
                    reservedMemoryIndices[j] = reservedMemoryIndices[j - 2];
                }
*/
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

#ifdef DEBUG
    for (size_t i = 0; i < numReservedMemoryIndices - 1; ++i)
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
    message("Total Allocation: %s - AvailableMemory %s - Num reserved blocks\n",
            allocationSize, availableMemory, reservedMemoryInfo.size());
    message("Memory block handles\n-------------\n");
    for (std::map<void*, MemoryBlockInfo>::iterator it = reservedMemoryInfo.begin();
         it != reservedMemoryInfo.end();
         ++it)
    {
        message("%s\n", it->first);
    }
    message("Memory Indices(%s)\n-------------\n", numReservedMemoryIndices);
    for (size_t i = 0; i < numReservedMemoryIndices; ++i)
    {
        message("%s\n", reservedMemoryIndices[i]);
        
    }
    message("\n");
}

void Memory::visualize()
{
    g_renderer->submitText("MEMORY", Vec2(-0.9f, 0.34f));
    g_renderer->submitText(String("Num allocations: " + toString(reservedMemoryInfo.size())), Vec2(-0.9f, 0.27f));
    g_renderer->submitText("Total Bytes: " + toString(allocationSize), Vec2(-0.9f, 0.24f));
    g_renderer->submitText("Reserved Bytes: " + toString(allocationSize - availableMemory), Vec2(-0.9f, 0.21f));
    g_renderer->submitText("Available Bytes: " + toString(availableMemory), Vec2(-0.9f, 0.18f));
    static Mat3 view = Mat3::view(Vec2(), 0, 1, g_aspect);
    g_renderer->setView(view);
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
        Quad blockQuad(barQuad.point0 + blockStart, barQuad.point1 + blockStart,
                       barQuad.point1 + blockEnd, barQuad.point0 + blockEnd);
        g_renderer->submitQuad(blockQuad, Vec2(), Vec4(1, 0, 0, 1));
    }
}
