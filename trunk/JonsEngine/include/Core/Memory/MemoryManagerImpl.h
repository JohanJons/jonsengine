#ifndef _MEMORY_MANAGER_IMPL_H
#define _MEMORY_MANAGER_IMPL_H


#include "../../../Interface/IMemoryManager.h"
#include "../../../Interface/ILogManager.h"
#include "../../../Interface/EngineDefs.h"

#include "Allocator.h"


namespace JonsEngine
{

	class MemoryManagerImpl : public IMemoryManager
	{
	public:
		MemoryManagerImpl();
		~MemoryManagerImpl();

		bool Init(ILogManager* logger);
		bool Init(Allocator_BackEnd allocatorBackEnd, ILogManager* logger);
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();
		
		void* Allocate(size_t size, Allocation_Mode mode = USER);
		void* ReAllocate(void* p, size_t size);
		void DeAllocate(void* obj, size_t size, Allocation_Mode mode = USER);

		inline Allocator* GetAllocator() { return mAllocator; }
		inline uint64_t GetTotalAllocatedMemory() { return (mUserAllocatedMemory + mInternalAllocatedMemory); }
		inline uint64_t GetUserAllocatedMemory() { return mUserAllocatedMemory; }
		inline uint64_t GetInternalAllocatedMemory() { return mInternalAllocatedMemory; }

	private:
		ILogManager* mLog;
		Allocator* mAllocator;

		bool mRunning;
		bool mInitialized;
		Allocator_BackEnd mAllocatorBackEnd;
		uint64_t mUserAllocatedMemory;
		uint64_t mInternalAllocatedMemory;

	};

}

#endif
