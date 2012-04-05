#include "../../../Include/Core/Memory/MemoryManagerImpl.h"

namespace JonsEngine
{

	MemoryManagerImpl::MemoryManagerImpl(ILogManager* logger) : 
							mTotalAllocatedObjects(0), mCurrentAllocatedObjects(0), mRunning(false),
							mUseDefaultMalloc(false), mLog(logger)
	{

	}

	MemoryManagerImpl::~MemoryManagerImpl()
	{
		Destroy();
	}

	int32_t MemoryManagerImpl::Init(bool UseDefaultMalloc)
	{
		if (UseDefaultMalloc)
		{
			mUseDefaultMalloc = UseDefaultMalloc;
		}
		return Init();
	}

	int32_t MemoryManagerImpl::Init()
	{
		if (mLog)
			return INIT_OK;
		else
			return INIT_NOK;
	}

	bool MemoryManagerImpl::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		res &= !mRunning;

		return res;
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

		if (mUseDefaultMalloc)
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

		if (mUseDefaultMalloc)
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
			if (mUseDefaultMalloc)
				free(obj);
			else
				dlfree(obj);

			mCurrentAllocatedObjects--;
		}
		else
			mLog->LogError() << "MemoryManagerImpl::Deallocate: object to deallocate has invalid address" << std::endl;
	}

}