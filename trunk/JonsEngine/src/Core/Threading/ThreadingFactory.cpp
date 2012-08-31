#include "include/Core/Threading/ThreadingFactory.h"

namespace JonsEngine
{
	ThreadingFactory::ThreadingFactory(ILogManager& logger, IMemoryAllocator& memAllocator) : mMemoryAllocator(memAllocator), mLogger(logger), 
										mCreatedThreads(memAllocator), mCreatedMutexes(memAllocator), mCreatedCondVars(memAllocator)
	{

	}
	
	ThreadingFactory::~ThreadingFactory()
	{
		Destroy();
	}

	bool ThreadingFactory::Init()
	{
		bool ret = true;

		return ret;
	}
		
	bool ThreadingFactory::Destroy()
	{
		bool ret = true;

		return ret;
	}

	IThread* const ThreadingFactory::CreateThread() 
	{
		IThread* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Thread, IMemoryAllocator&, ILogManager&>(mMemoryAllocator, mLogger);

		return ret;
	}

	IThread* const ThreadingFactory::CreateThread(IThread::Task task, void* arg)
	{
		IThread* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Thread, IThread::Task, void*, IMemoryAllocator&, ILogManager&>(task, arg, mMemoryAllocator, mLogger);

		return ret;
	}
		
	IMutex* const ThreadingFactory::CreateMutex()
	{
		IMutex* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Mutex, ILogManager&>(mLogger);

		return ret;
	}
		
	IConditionVariable* const ThreadingFactory::CreateConditionVariable()
	{
		IConditionVariable* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<ConditionVariable, ILogManager&>(mLogger);

		return ret;
	}

	IThreadPool* const ThreadingFactory::CreateThreadPool(uint32_t numThreads)
	{
		IThreadPool* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<ThreadPool, IMemoryAllocator&, ILogManager&, uint32_t>(mMemoryAllocator, mLogger, numThreads);

		return ret;
	}
		
	void ThreadingFactory::DestroyThread(IThread* const thread)
	{

		mMemoryAllocator.DeallocateObject<IThread>(thread);
	}
		
	void ThreadingFactory::DestroyMutex(IMutex* const mutex)
	{
		mMemoryAllocator.DeallocateObject<IMutex>(mutex);
	}
		
	void ThreadingFactory::DestroyConditionVariable(IConditionVariable* const condVar)
	{
		mMemoryAllocator.DeallocateObject<IConditionVariable>(condVar);
	}

	void ThreadingFactory::DestroyThreadPool(IThreadPool* const thread)
	{
		mMemoryAllocator.DeallocateObject<IThreadPool>(thread);
	}
}