#include "include/Core/Threading/ThreadPool.h"

namespace JonsEngine
{
	ThreadPool::ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, uint32_t numThreads) : mMemoryAllocator(allocator), mLogger(logger), mNumThreads(numThreads)
	{

	}

	ThreadPool::~ThreadPool()
	{
		
	}

	void ThreadPool::AddTask(Task task)
	{

	}
	
	void ThreadPool::Wait()
	{

	}

	uint32_t ThreadPool::GetNumThreads() const
	{
		return mNumThreads;
	}

}