#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include <malloc.h>

#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

#include "../../../interface/ILogManager.h"

namespace JonsEngine
{
	class HeapAllocator
	{
	friend class MemoryManagerImpl;

	public:
		HeapAllocator();
		~HeapAllocator();

		void* Allocate(size_t size);
        void* ReAllocate(void* p, size_t size);
        void DeAllocate(void* p);

		uint64_t GetAllocatedMemory();

	private:
		ILogManager* mLog;
	};
}


#endif