#ifndef _MEMORY_MANAGER_IMPL_H
#define _MEMORY_MANAGER_IMPL_H

#include <malloc.h>
#include <vector>

#include "../../../Interface/IMemoryManager.h"
#include "../../../Interface/ILogManager.h"
#include "../../../Interface/EngineDefs.h"

#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

namespace JonsEngine
{

	class MemoryManagerImpl : public IMemoryManager
	{
	public:
		MemoryManagerImpl();
		~MemoryManagerImpl();

		bool Init(ILogManager* logger);
		bool Init(bool UseDLMallocm, ILogManager* logger);
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
		bool mInitialized;
		bool mUseDLMalloc;
		uint64_t mTotalAllocatedObjects;
		uint64_t mCurrentAllocatedObjects;

	};

}

#endif
