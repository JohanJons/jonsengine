#include "../../../Include/Core/Memory/MemoryManagerImpl.h"

namespace JonsEngine
{

	MemoryManagerImpl::MemoryManagerImpl() : 
							mUserAllocatedMemory(0), mInternalAllocatedMemory(0), mRunning(false),
							mAllocatorBackEnd(DLMALLOC), mInitialized(false), mLog(NULL), mAllocator(NULL)
	{

	}

	MemoryManagerImpl::~MemoryManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool MemoryManagerImpl::Init(Allocator_BackEnd allocatorBackEnd, ILogManager* logger)
	{
		mAllocatorBackEnd = allocatorBackEnd;

		return Init(logger);
	}

	bool MemoryManagerImpl::Init(ILogManager* logger)
	{
		mLog = logger;

		if (mAllocatorBackEnd == DLMALLOC)
			mAllocator =  new (dlmalloc(sizeof(Allocator))) Allocator(mAllocatorBackEnd);
		else if (mAllocatorBackEnd == DEFAULT_MALLOC)
			mAllocator = new Allocator(mAllocatorBackEnd);

		if (mLog && mAllocator)
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

			if (mAllocatorBackEnd == DLMALLOC)
			{
				mAllocator->~Allocator();
				dlfree(mAllocator);
				mAllocator = NULL;
			}
			else if (mAllocatorBackEnd == DEFAULT_MALLOC)
			{
				delete mAllocator;
				mAllocator = NULL;
			}
			else 
				res &= false;

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

	void* MemoryManagerImpl::Allocate(size_t size, Allocation_Mode mode)
	{
		void* alloc = NULL;

		if (mAllocator)
		{
			alloc = mAllocator->Allocate(size);
		
			if (alloc)
			{
				if (mode == USER) mUserAllocatedMemory += size;
				if (mode == INTERNAL) mInternalAllocatedMemory += size;
			}
			else
				mLog->LogError() << "MemoryManagerImpl::Allocate: Allocator not valid!" << std::endl;
		}
		return alloc;
	}
	
	void* MemoryManagerImpl::ReAllocate(void* p, size_t size)
	{
		if (mAllocator)
		{
			void* alloc = mAllocator->ReAllocate(p,size);

			if (!alloc)
				mLog->LogError() << "MemoryManagerImpl::Allocate: Unable to reallocate memory!" << std::endl;

			return alloc;
		}
		else 
		{
			mLog->LogError() << "MemoryManagerImpl::ReAllocate: Allocator not valid!" << std::endl;
			return NULL;
		}
	}

	void MemoryManagerImpl::DeAllocate(void* obj,size_t size, Allocation_Mode mode)
	{
		if (!obj)
		{
			mLog->LogError() << "MemoryManagerImpl::DeAllocate: Object not valid!" << std::endl;
			return;
		}


		if (!mAllocator)
		{
			mLog->LogError() << "MemoryManagerImpl::DeAllocate: Allocator not valid!" << std::endl;
			return;
		}

		mAllocator->DeAllocate(obj);

		if (mode == USER) mUserAllocatedMemory -= size;
		if (mode == INTERNAL) mInternalAllocatedMemory -= size;
	}

}