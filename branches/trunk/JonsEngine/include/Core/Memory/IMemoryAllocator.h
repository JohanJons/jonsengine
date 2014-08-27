#pragma once

#include "include/Core/EngineDefs.h"

#include <utility>
#include <memory>

namespace JonsEngine
{
    class IMemoryAllocator;

    /*
     * Common interface for memory allocator schemes such as HeapAllocator
     */

    /*
     * IMemoryAllocatorPtr definition
     */
    typedef std::shared_ptr<IMemoryAllocator> IMemoryAllocatorPtr;

    /* IMemoryAllocator definition */
    class IMemoryAllocator
    {
    public:
        virtual ~IMemoryAllocator() { }

        /**
         * Allocates a block of memory.
         * @param size: memoryblock size in bytes to allocate. If NULL, no memory will be allocated.
         * @return: address of the allocated memoryblock if successfull, otherwise NULL
         */
        virtual void* Allocate(size_t size) = 0;

        /**
         * Reallocates a block of memory.
         * @param memblock: address of the memory block to reallocate. If NULL, no memory will be reallocated.
         * @param size: memoryblock size in bytes to reallocate. If NULL, no memory will be reallocated.
         * @return: address of the reallocated memoryblock if successfull, otherwise NULL
         */
        virtual void* Reallocate(void* memblock, size_t size) = 0;

        /**
         * Deallocates a block of memory.
         * @param memblock: address of the memory block to be deallocated. If NULL, no memory will be deallocated.
         */
        virtual void Deallocate(void* memblock) = 0;

        

        /**
         * Allocates a given object and passes on given arguments to its constructor.
         * @param [...]: arguments that will be passed to the constructor of the object allocated.
         * @return: address of the constructed object if successfull, otherwise NULL.
         */
        template <class T, typename... Arguments>
        inline T* AllocateObject(Arguments&&... args) { return new (InternalAllocate(sizeof(T))) T(std::forward<Arguments>(args)...); }

        /**
         * Calls the destructor and deallocates memory of a object
         * @param obj: pointer to the object to deallocate. If NULL, does nothing.
         */
        template <class T>
        inline void DeallocateObject(T* obj)
        {
            obj->~T();
            InternalDeallocate(obj);
        }

        /**
         * Returns the total amount of allocated memory in bytes
         * @return: The total amount of memory allocated.
         */
        virtual uint64_t GetAllocatedMemory() const = 0;

        /**
         * Returns the name of the memory allocator.
         * @return: name of the allocator.
         */
        virtual const std::string& GetAllocatorName() const = 0;




    private:
        virtual void* InternalAllocate(size_t size) = 0;
        virtual void InternalDeallocate(void* p) = 0;
    };
}