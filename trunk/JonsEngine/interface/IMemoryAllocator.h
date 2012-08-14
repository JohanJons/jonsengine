#ifndef _I_MEMORY_ALLOCATOR_H
#define _I_MEMORY_ALLOCATOR_H

#include "EngineDefs.h"

namespace JonsEngine
{
	class IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() { }

		virtual void* Allocate(size_t size) = 0;
        virtual void* Reallocate(void* p, size_t size) = 0;
        virtual void Deallocate(void* p) = 0;

		virtual uint64_t GetAllocatedMemory() const = 0;
	};

}

#endif
