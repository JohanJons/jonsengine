#ifndef _MEMORY_MANAGER_IMPL_H
#define _MEMORY_MANAGER_IMPL_H

#include <malloc.h>

#include "../../../Interface/IMemoryManager.h"
#include "../../../Interface/ILogManager.h"
#include "../../../Interface/EngineDefs.h"

#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

namespace JonsEngine
{

	class MemoryManagerImpl : public IMemoryManager
	{
	public:
		MemoryManagerImpl(ILogManager* logger);
		~MemoryManagerImpl();

		int32_t Init();
		int32_t Init(bool UseDefaultMalloc);
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();
		
		void* Allocate(size_t size);
		void* ReAllocate(void* p, size_t size);
		void Deallocate(void* obj);

		inline uint64_t GetTotalAllocatedObjectsCount() { return mTotalAllocatedObjects; }
		inline uint64_t GetCurrentAllocatedObjectsCount() { return mCurrentAllocatedObjects; }

	private:
		ILogManager* mLog;

		bool mRunning;
		bool mUseDefaultMalloc;
		uint64_t mTotalAllocatedObjects;
		uint64_t mCurrentAllocatedObjects;

	};

}

#endif
