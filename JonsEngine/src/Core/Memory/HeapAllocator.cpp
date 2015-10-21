#include "include/Core/Memory/HeapAllocator.h"

#include "../Thirdparty/dlmalloc/dlmalloc.h"


namespace JonsEngine
{
    static HeapAllocator gHeapAllocator("DefaultHeapAllocator");


    HeapAllocator::HeapAllocator(const std::string& allocatorName) : mAllocatorName(allocatorName)
    {
    }

    HeapAllocator::~HeapAllocator()
    {
        dlmalloc_trim(0);
    }

    HeapAllocator& HeapAllocator::GetDefaultHeapAllocator()
    {
        return gHeapAllocator;
    }


    void* HeapAllocator::Allocate(const size_t size)
    {
        return dlmalloc(size);
    }

    void* HeapAllocator::Allocate(const size_t size, const size_t alignment)
    {
        return dlmemalign(size, alignment);
    }

    void* HeapAllocator::Reallocate(void* memblock, const size_t size)
    {
        return dlrealloc(memblock, size);
    }

    void HeapAllocator::Deallocate(void* memblock)
    {
        assert(memblock);

        dlfree(memblock);
    }


    uint64_t HeapAllocator::GetAllocatedMemory() const
    {
        struct mallinfo mi = dlmallinfo();

        return (mi.arena + mi.hblkhd);
    }

    const std::string& HeapAllocator::GetAllocatorName() const
    {
        return mAllocatorName;
    }
}