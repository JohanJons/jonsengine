#ifndef _HEAP_ALLOCATOR_H
#define _HEAP_ALLOCATOR_H

#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

#include "../../../interface/ILogManager.h"
#include "../../../interface/IMemoryAllocator.h"

namespace JonsEngine
{
	class HeapAllocator : public IMemoryAllocator
	{
	public:
		enum HeapAllocatorBackend
		{
			SYSTEM_DEFAULT = 0,
			DLMALLOC
		};

		HeapAllocator(HeapAllocatorBackend backend);
		~HeapAllocator();

		void* Allocate(size_t size);
        void* Reallocate(void* p, size_t size);
        void Deallocate(void* p);

		uint64_t GetAllocatedMemory();

		void SetLogger(ILogManager* const logger);

	private:
		HeapAllocatorBackend mBackend;
		ILogManager* mLog;
	};

	// using system defaults (malloc/free/realloc)
	static HeapAllocator SYSTEM_DEFAULT_HEAP_ALLOCATOR(HeapAllocator::SYSTEM_DEFAULT);
}



#endif