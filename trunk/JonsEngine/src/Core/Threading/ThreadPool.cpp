#include "include/Core/Threading/ThreadPool.h"

#include "interface/Core/Threading/IThreadingFactory.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
	ThreadPool::ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, IThreadingFactory& factory, uint32_t initialNumThreads) : mMemoryAllocator(allocator), mLogger(logger), mFactory(factory),
																																			mNumThreads(0), mScheduledTasks(allocator)
	{
		SetNumThreads(initialNumThreads);
	}

	ThreadPool::~ThreadPool()
	{
		
	}

	void ThreadPool::AddTask(Task task)
	{

	}

	void ThreadPool::ClearTasks()
	{

	}

	size_t ThreadPool::PendingTasks() const
	{
		return mScheduledTasks.size();
	}

	bool ThreadPool::Empty() const
	{
		return mScheduledTasks.empty();
	}
	
	void ThreadPool::Wait()
	{

	}

	void ThreadPool::SetNumThreads(uint32_t num)
	{
		while (mNumThreads < num)
		{
			mFactory.CreateThread(boost::bind(&ThreadPool::Worker, this, _1), NULL);
			mNumThreads++;
		}
	}

	uint32_t ThreadPool::GetNumThreads() const
	{
		return mNumThreads;
	}

	void ThreadPool::Worker(void* arg)
	{
		jons_SleepCurrentThread(3000);

		int i = 2;
	}
}