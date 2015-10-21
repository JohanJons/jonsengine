#pragma once

#include <string.h>

namespace JonsEngine
{
    /*
     * HeapAllocator definition
     * Allocates memory on the heap using either system default or DLMALLOC allocation strategy
     */
    class HeapAllocator
    {
    public:
        HeapAllocator(const std::string& allocatorName);
        ~HeapAllocator();
        static HeapAllocator& GetDefaultHeapAllocator();

        void* Allocate(const size_t size);
        void* Allocate(const size_t size, const size_t alignment);
        void* Reallocate(void* memblock, const size_t size);
        void Deallocate(void* memblock);

        template <class T, typename... Arguments>
        T* AllocateObject(Arguments&&... args);
        template <class T>
        void DeallocateObject(T* obj);

        uint64_t GetAllocatedMemory() const;
        const std::string& GetAllocatorName() const;


    private:
        const std::string mAllocatorName;
    };


    template <class T, typename... Arguments>
    T* HeapAllocator::AllocateObject(Arguments&&... args)
    {
        return new (Allocate(sizeof(T))) T(std::forward<Arguments>(args)...);
    }

    template <class T>
    void HeapAllocator::DeallocateObject(T* obj)
    {
        assert(obj);

        obj->~T();
        Deallocate(obj);
    }
}