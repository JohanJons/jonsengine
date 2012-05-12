#ifndef _MEMORY_MANAGER_IMPL_H
#define _MEMORY_MANAGER_IMPL_H


#include "../../../Interface/IMemoryManager.h"
#include "../../../Interface/ILogManager.h"
#include "../../../Interface/EngineDefs.h"

#include "HeapAllocator.h"

namespace JonsEngine
{

	class MemoryManagerImpl : public IMemoryManager
	{
	public:
		MemoryManagerImpl();
		~MemoryManagerImpl();

		bool Init(ILogManager* logger);
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();

		inline HeapAllocator* GetHeapAllocator() { return &mHeapAllocator; }
		uint64_t GetTotalAllocatedMemory();

	private:
		void* InternalAllocate(size_t size);
		void InternalDeAllocate(void* p);

		ILogManager* mLog;
		HeapAllocator mHeapAllocator;

		bool mRunning;
		bool mInitialized;

	};

}

#endif
