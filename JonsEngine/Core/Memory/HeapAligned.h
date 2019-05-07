#pragma once

#include "Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
    template<size_t memoryAlignment>
    class HeapAligned
    {
    public:
        static void* operator new(const size_t size)
        {
            return HeapAllocator::GetDefaultHeapAllocator().Allocate(size, memoryAlignment);
        }

        static void operator delete(void* memory)
        {
            assert(memory);
            HeapAllocator::GetDefaultHeapAllocator().Deallocate(memory);
        }
    };
}