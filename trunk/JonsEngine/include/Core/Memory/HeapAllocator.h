#pragma once

#include "interface/Core/Memory/IMemoryAllocator.h"

#include "../Thirdparty/dlmalloc/dlmalloc.h"

#include <string.h>


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

		HeapAllocator(const std::string& allocatorName, const HeapAllocatorBackend backend);
		~HeapAllocator();

		void* Allocate(size_t size);
        void* Reallocate(void* memblock, size_t size);
        void Deallocate(void* memblock);

		uint64_t GetAllocatedMemory() const;
		const std::string& GetAllocatorName() const;

	private:
		void* InternalAllocate(size_t size);
		void InternalDeallocate(void* memblock);

		const HeapAllocatorBackend mBackend;
		const std::string& mAllocatorName;
	};
}