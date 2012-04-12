#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include <malloc.h>

#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

#include "../../../interface/EngineDefs.h"

namespace JonsEngine
{
	class Allocator
	{
	public:
		Allocator(Allocator_BackEnd mode);
		~Allocator();

		void* Allocate(size_t n);
        void* ReAllocate(void* p, size_t n);
        void DeAllocate(void* p);

	private:
		Allocator_BackEnd mAllocatorBackEnd;
	};
}


#endif