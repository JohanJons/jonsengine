#include "include/Core/Memory/MemoryManagerImpl.h"

namespace JonsEngine
{

	MemoryManagerImpl::MemoryManagerImpl() : mRunning(false), mInitialized(false), mLog(NULL), mHeapAllocator(HeapAllocator::DLMALLOC)
	{

	}

	MemoryManagerImpl::~MemoryManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool MemoryManagerImpl::Init(ILogManager* const logger)
	{
		mLog = logger;

		mHeapAllocator.SetLogger(logger);

		if (mLog)
		{
			mInitialized = true;
			return true;
		}
		else
			return false;
	}

	bool MemoryManagerImpl::Destroy()
	{
		if (mInitialized)
		{
			bool res = true;

			if (mRunning)
				res &= Stop();

			res &= !mRunning;

			mInitialized = false;

			return res;
		}
		else
			return false;
	}

	bool MemoryManagerImpl::Start()
	{
		bool res = true;

		if (!mRunning)
		{
			mRunning = true;

		}
		else
			mLog->LogWarn() <<  "MemoryManagerImpl::Start(): MemoryManager already started!" << std::endl;

		return res;
	}

	bool MemoryManagerImpl::Stop()
	{
		bool res = true;

		if (mRunning)
		{
			mRunning = false;

		}
		else
			mLog->LogWarn() <<  "MemoryManagerImpl::Stop(): MemoryManager already stopped!" << std::endl;

		return res;
	}

	bool MemoryManagerImpl::isRunning()
	{
		return mRunning;
	}

	void MemoryManagerImpl::Tick()
	{

	}
	 
	uint64_t MemoryManagerImpl::GetTotalAllocatedMemory() const
	{
		uint64_t totalAllocatedMemory = 0;

		// += heap memory
		totalAllocatedMemory += mHeapAllocator.GetAllocatedMemory();

		//...

		return totalAllocatedMemory;
	}

	void* MemoryManagerImpl::InternalAllocate(size_t size)
	{
		return mHeapAllocator.Allocate(size);
	}

	void MemoryManagerImpl::InternalDeAllocate(void* p)
	{ 
		mHeapAllocator.Deallocate(p);
	}

}