#include "../../../Include/Core/Memory/MemoryManagerImpl.h"

namespace JonsEngine
{

	MemoryManagerImpl::MemoryManagerImpl() : 
							mTotalAllocatedObjects(0), mCurrentAllocatedObjects(0), mRunning(false),
							mUseDLMalloc(true), mInitialized(false), mLog(NULL)
	{

	}

	MemoryManagerImpl::~MemoryManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool MemoryManagerImpl::Init(bool UseDLMalloc, ILogManager* logger)
	{
		mUseDLMalloc = UseDLMalloc;

		return Init(logger);
	}

	bool MemoryManagerImpl::Init(ILogManager* logger)
	{
		mLog = logger;

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

	void* MemoryManagerImpl::Allocate(size_t size)
	{
		void* alloc;

		if (mUseDLMalloc)
			alloc = dlmalloc(size);
		else
			alloc = malloc(size);

		if (alloc)
		{
			mTotalAllocatedObjects++;
			mCurrentAllocatedObjects++;
		}
		else
			mLog->LogError() << "MemoryManagerImpl::Allocate: Unable to allocate memory!" << std::endl;

		return alloc;
	}
	
	void* MemoryManagerImpl::ReAllocate(void* p, size_t size)
	{
		void* alloc;

		if (mUseDLMalloc)
			alloc = dlrealloc(p,size);
		else
			alloc = realloc(p,size);

		if (!alloc)
			mLog->LogError() << "MemoryManagerImpl::Allocate: Unable to allocate memory!" << std::endl;

		return alloc;
	}

	void MemoryManagerImpl::Deallocate(void* obj)
	{
		if (obj)
		{
			if (mUseDLMalloc)
				dlfree(obj);
			else
				free(obj);

			mCurrentAllocatedObjects--;
		}
		else
			mLog->LogError() << "MemoryManagerImpl::Deallocate: object to deallocate has invalid address" << std::endl;
	}

}