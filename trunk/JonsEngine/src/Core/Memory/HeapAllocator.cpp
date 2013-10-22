#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
    static HeapAllocator gHeapAllocator("DefaultHeapAllocator", HeapAllocator::DLMALLOC);


    HeapAllocator::HeapAllocator(const std::string& allocatorName) : HeapAllocator(allocatorName, HeapAllocator::DLMALLOC)
    {
    }

    HeapAllocator::HeapAllocator(const std::string& allocatorName, HeapAllocatorBackend backend) : mAllocatorName(allocatorName), mBackend(backend)
    {
    }
    
    HeapAllocator::~HeapAllocator()
    {
        dlmalloc_trim(0);
    }

    IMemoryAllocator& HeapAllocator::GetDefaultHeapAllocator()
    {
        return gHeapAllocator;
    }

    void* HeapAllocator::Allocate(size_t size)
    {
        void* alloc = NULL;

        if (mBackend == HeapAllocator::DLMALLOC && size)
            alloc = dlmalloc(size);
        else if (mBackend == HeapAllocator::SYSTEM_DEFAULT && size)
            alloc = malloc(size);

        return alloc;
    }

    void* HeapAllocator::Reallocate(void* memblock, size_t size)
    {
        void* alloc = NULL;

        if (mBackend == HeapAllocator::DLMALLOC && memblock && size)
            alloc = dlrealloc(memblock, size);
        else if (mBackend == HeapAllocator::SYSTEM_DEFAULT && memblock && size)
            alloc = realloc(memblock, size);
        
        return alloc;
    }

    void HeapAllocator::Deallocate(void* memblock)
    {
        if (memblock)
        {
            if (mBackend == HeapAllocator::DLMALLOC)
                dlfree(memblock);
            else if (mBackend == HeapAllocator::SYSTEM_DEFAULT)
                free(memblock);
        }
    }

    uint64_t HeapAllocator::GetAllocatedMemory() const
    {

        if (mBackend == HeapAllocator::DLMALLOC)
        {
            struct mallinfo mi = dlmallinfo();

            return (mi.arena + mi.hblkhd);
        }
        else if (mBackend == HeapAllocator::SYSTEM_DEFAULT)
        {
            return 0;
        }
        else
            return 0;

    }

    void* HeapAllocator::InternalAllocate(size_t size)
    {
        return Allocate(size);
    }

    void HeapAllocator::InternalDeallocate(void* memblock)
    { 
        Deallocate(memblock);
    }
}