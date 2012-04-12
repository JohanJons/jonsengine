#include "../../../Include/Core/Memory/Allocator.h"

namespace JonsEngine
{
	Allocator::Allocator(Allocator_BackEnd backend) : mAllocatorBackEnd(backend)
	{
	}
	
	Allocator::~Allocator()
	{
	}

	void* Allocator::Allocate(size_t n)
	{
		void* alloc;

		if (mAllocatorBackEnd == DLMALLOC)
			alloc = dlmalloc(n);
		else if (mAllocatorBackEnd == DEFAULT_MALLOC)
			alloc = malloc(n);

		return alloc;
	}

    void* Allocator::ReAllocate(void* p, size_t n)
	{
		void* alloc;

		if (mAllocatorBackEnd == DLMALLOC)
			alloc = dlrealloc(p,n);
		else if (mAllocatorBackEnd == DEFAULT_MALLOC)
			alloc = realloc(p,n);

		return alloc;
	}

    void Allocator::DeAllocate(void* p)
	{
		if (p)
		{
			if (mAllocatorBackEnd == DLMALLOC)
				dlfree(p);
			else if (mAllocatorBackEnd == DEFAULT_MALLOC)
				free(p);
		}
	}

}