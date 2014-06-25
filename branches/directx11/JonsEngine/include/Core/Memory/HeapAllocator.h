#pragma once

#include "include/Core/Memory/IMemoryAllocator.h"

#include "../Thirdparty/dlmalloc/dlmalloc.h"

#include <string.h>

namespace JonsEngine
{
    /*
     * HeapAllocator definition
     * Allocates memory on the heap using either system default or DLMALLOC allocation strategy
     */
    class HeapAllocator : public IMemoryAllocator
    {
    public:
        enum HeapAllocatorBackend
        {
            SYSTEM_DEFAULT = 0,
            DLMALLOC
        };

        HeapAllocator(const std::string& allocatorName);
        HeapAllocator(const std::string& allocatorName, const HeapAllocatorBackend backend);
        ~HeapAllocator();
        static IMemoryAllocator& GetDefaultHeapAllocator();

        void* Allocate(size_t size);
        void* Reallocate(void* memblock, size_t size);
        void Deallocate(void* memblock);

        uint64_t GetAllocatedMemory() const;
        const std::string& GetAllocatorName() const;

    private:
        void* InternalAllocate(size_t size);
        void InternalDeallocate(void* memblock);

        const HeapAllocatorBackend mBackend;
        const std::string mAllocatorName;
    };


    /* HeapAllocator inlines */
    inline const std::string& HeapAllocator::GetAllocatorName() const      { return mAllocatorName; }
}