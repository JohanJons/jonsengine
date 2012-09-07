#include "include/Core/Threading/ThreadingFactory.h"

#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Memory/IMemoryAllocator.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/ConditionVariable.h"
#include "include/Core/Threading/Mutex.h"
#include "include/Core/Threading/ThreadPool.h"

namespace JonsEngine
{
	ThreadingFactory::ThreadingFactory(ILogManager& logger, IMemoryAllocator& memAllocator) : mMemoryAllocator(memAllocator), mLogger(logger), 
										mCreatedThreads(memAllocator), mCreatedMutexes(memAllocator), mCreatedCondVars(memAllocator), mCreatedThreadPools(memAllocator)
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

		// clear threads
		for (Vector<IThread*>::iterator iter = mCreatedThreads.begin(); iter != mCreatedThreads.end(); iter = mCreatedThreads.begin())
			DestroyThread(*iter);

		// clear mutexes
		for (Vector<IMutex*>::iterator iter = mCreatedMutexes.begin(); iter != mCreatedMutexes.end(); iter = mCreatedMutexes.begin())
			DestroyMutex(*iter);

		// clear condvars
		for (Vector<IConditionVariable*>::iterator iter = mCreatedCondVars.begin(); iter != mCreatedCondVars.end(); iter = mCreatedCondVars.begin())
			DestroyConditionVariable(*iter);

		// clear threadpools
		for (Vector<IThreadPool*>::iterator iter = mCreatedThreadPools.begin(); iter != mCreatedThreadPools.end(); iter = mCreatedThreadPools.begin())
			DestroyThreadPool(*iter);

		return ret;
	}

	IThread* const ThreadingFactory::CreateThread() 
	{
		IThread* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Thread, IMemoryAllocator&, ILogManager&>(mMemoryAllocator, mLogger);

		if (ret != NULL)
			mCreatedThreads.push_back(ret);

		return ret;
	}

	IThread* const ThreadingFactory::CreateThread(Task task)
	{
		IThread* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Thread, Task, IMemoryAllocator&, ILogManager&>(task, mMemoryAllocator, mLogger);

		if (ret != NULL)
			mCreatedThreads.push_back(ret);

		return ret;
	}
		
	IMutex* const ThreadingFactory::CreateMutex()
	{
		IMutex* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<Mutex, ILogManager&>(mLogger);

		if (ret != NULL)
			mCreatedMutexes.push_back(ret);

		return ret;
	}
		
	IConditionVariable* const ThreadingFactory::CreateConditionVariable()
	{
		IConditionVariable* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<ConditionVariable, ILogManager&>(mLogger);

		if (ret != NULL)
			mCreatedCondVars.push_back(ret);

		return ret;
	}

	IThreadPool* const ThreadingFactory::CreateThreadPool(uint32_t initialNumThreads)
	{
		IThreadPool* ret = NULL;

		ret = mMemoryAllocator.AllocateObject<ThreadPool, IMemoryAllocator&, ILogManager&, IThreadingFactory&, uint32_t>(mMemoryAllocator, mLogger, *this, initialNumThreads);

		if (ret != NULL)
			mCreatedThreadPools.push_back(ret);

		return ret;
	}
		
	void ThreadingFactory::DestroyThread(IThread* const thread)
	{
		if (thread)
		{
			mMemoryAllocator.DeallocateObject<IThread>(thread);

			for (Vector<IThread*>::iterator iter = mCreatedThreads.begin(); iter != mCreatedThreads.end(); iter++)
			{
				if (*iter == thread)
				{
					mCreatedThreads.erase(iter);
					break;
				}
			}
		}
	}
		
	void ThreadingFactory::DestroyMutex(IMutex* const mutex)
	{
		if (mutex)
		{
			mMemoryAllocator.DeallocateObject<IMutex>(mutex);

			for (Vector<IMutex*>::iterator iter = mCreatedMutexes.begin(); iter != mCreatedMutexes.end(); iter++)
			{
				if (*iter == mutex)
				{
					mCreatedMutexes.erase(iter);
					break;
				}
			}
		}
	}
		
	void ThreadingFactory::DestroyConditionVariable(IConditionVariable* const condVar)
	{
		if (condVar)
		{
			mMemoryAllocator.DeallocateObject<IConditionVariable>(condVar);

			for (Vector<IConditionVariable*>::iterator iter = mCreatedCondVars.begin(); iter != mCreatedCondVars.end(); iter++)
			{
				if (*iter == condVar)
				{
					mCreatedCondVars.erase(iter);
					break;
				}
			}
		}
	}

	void ThreadingFactory::DestroyThreadPool(IThreadPool* const tp)
	{
		if (tp)
		{
			mMemoryAllocator.DeallocateObject<IThreadPool>(tp);

			for (Vector<IThreadPool*>::iterator iter = mCreatedThreadPools.begin(); iter != mCreatedThreadPools.end(); iter++)
			{
				if (*iter == tp)
				{
					mCreatedThreadPools.erase(iter);
					break;
				}
			}
		}
	}
}