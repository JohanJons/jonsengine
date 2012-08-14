#ifndef _HEAP_ALLOCATOR_H
#define _HEAP_ALLOCATOR_H

#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Memory/IMemoryAllocator.h"

#include "../Thirdparty/dlmalloc/dlmalloc.h"

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

		uint64_t GetAllocatedMemory() const;

		void SetLogger(ILogManager* const logger);

	private:
		HeapAllocatorBackend mBackend;
		ILogManager* mLog;
	};

	// using system defaults (malloc/free/realloc)
	static HeapAllocator SYSTEM_DEFAULT_HEAP_ALLOCATOR(HeapAllocator::SYSTEM_DEFAULT);
}



#endif