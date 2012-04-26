#include "../../../Include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
	HeapAllocator::HeapAllocator() : mLog(NULL)
	{
	}
	
	HeapAllocator::~HeapAllocator()
	{
		dlmalloc_trim(0);
	}

	void* HeapAllocator::Allocate(size_t size)
	{
		void* alloc;

		alloc = dlmalloc(size);

		if(!alloc && mLog)
			mLog->LogError() << "HeapAllocator::Allocate: Unable to allocate memory!" << std::endl;

		return alloc;
	}

    void* HeapAllocator::ReAllocate(void* p, size_t size)
	{
		void* alloc;

		alloc = dlrealloc(p,size);

		if (!alloc && mLog)
			mLog->LogError() << "HeapAllocator::ReAllocate: Unable to reallocate memory!" << std::endl;

		return alloc;
	}

    void HeapAllocator::DeAllocate(void* p)
	{
		if (p)
			dlfree(p);
		else if (mLog)
			mLog->LogError() << "HeapAllocator::DeAllocate: Object not valid!" << std::endl;
	}

	uint64_t HeapAllocator::GetAllocatedMemory()
	{
		struct mallinfo mi = dlmallinfo();

		return (mi.arena + mi.hblkhd);
	}

}