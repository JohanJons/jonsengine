#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
	HeapAllocator::HeapAllocator(HeapAllocatorBackend backend) : mBackend(backend)
	{
	}
	
	HeapAllocator::~HeapAllocator()
	{
		dlmalloc_trim(0);
	}

	void* HeapAllocator::Allocate(size_t size)
	{
		void* alloc = NULL;

		if (mBackend == HeapAllocator::DLMALLOC)
			alloc = dlmalloc(size);
		else if (mBackend == HeapAllocator::SYSTEM_DEFAULT)
			alloc = malloc(size);

		return alloc;
	}

    void* HeapAllocator::Reallocate(void* p, size_t size)
	{
		void* alloc = NULL;

		if (mBackend == HeapAllocator::DLMALLOC)
			alloc = dlrealloc(p, size);
		else if (mBackend == HeapAllocator::SYSTEM_DEFAULT)
			alloc = realloc(p, size);
		
		return alloc;
	}

    void HeapAllocator::Deallocate(void* p)
	{
		if (p)
		{
			if (mBackend == HeapAllocator::DLMALLOC)
				dlfree(p);
			else if (mBackend == HeapAllocator::SYSTEM_DEFAULT)
				free(p);
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

	void HeapAllocator::InternalDeallocate(void* p)
	{ 
		Deallocate(p);
	}
}