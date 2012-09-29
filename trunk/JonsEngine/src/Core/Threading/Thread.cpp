#include "include/Core/Threading/Thread.h"

#include "include/Core/Memory/IMemoryAllocator.h"

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <process.h>
	#undef WIN32_LEAN_AND_MEAN
#elif defined ANDROID
	#include <time.h>
#endif


namespace JonsEngine
{
	/////////////////////////////////////////////////////
	/////				Free functions				/////
	/////////////////////////////////////////////////////

	void SleepCurrentThread(uint32_t milliseconds)
	{
		#if defined _WIN32 || _WIN64
			Sleep(milliseconds);
		#elif defined ANDROID
			timespec interval;
			interval.tv_sec = (long)(milliseconds/1000); 
			interval.tv_nsec = (long)(milliseconds%1000) * 1000000; 

			nanosleep(&interval, 0);
		#endif
	}



	/////////////////////////////////////////////////////////////
	/////				Thread implementation				/////
	/////////////////////////////////////////////////////////////

	Thread::Thread() : mHandle(NULL), mAllocator(Globals::GetDefaultHeapAllocator()), mThreadInfo(NULL)
	{
	}

	Thread::Thread(Task task) : mAllocator(Globals::GetDefaultHeapAllocator())
	{
		mThreadInfo = (ThreadInfo*) mAllocator.AllocateObject<ThreadInfo>();

		mThreadInfo->mState = Thread::RUNNING;
		mThreadInfo->mTask = task;

		mHandle = _CreateThread(&Run, (void*)mThreadInfo);
		if (!mHandle)
			Detach();
			
	
	}

	Thread::~Thread()
	{
		if (mThreadInfo != NULL && mThreadInfo->mState == Thread::RUNNING)
			Join();

		Destroy();
	}

	Thread& Thread::operator=(Thread& other)
	{
		if (&other != this)
		{
			mThreadInfo = other.mThreadInfo;
			mHandle = other.mHandle;
			mAllocator = other.mAllocator;

			other.Detach();
		}

		return *this;
	}

	int32_t Thread::Join()
	{
		if (mThreadInfo != NULL && mThreadInfo->mState == Thread::RUNNING)
		{
			_JoinThread(mHandle);
			return 0;
		}
		else
			return -1;
	}

	int32_t Thread::SetPriority(int32_t priority)
	{
		if (mThreadInfo != NULL && mThreadInfo->mState == Thread::RUNNING && mHandle != NULL)
			return jons_SetThreadPriority(mHandle, priority);
		else
			return -1;
	}

	void* Thread::Run(void* arg)
	{
		ThreadInfo* thread = static_cast<ThreadInfo*>(arg);

		if (thread)
		{
			thread->mTask();
			
			thread->mState = Thread::FINISHED;
		}

		return NULL;
	}

	Thread::ThreadState Thread::GetThreadState() const
	{
		if (mThreadInfo != NULL)
			return mThreadInfo->mState;
		else
			return Thread::DETACHED;
	}

	int32_t Thread::jons_SetThreadPriority(Thread::ThreadHandle handle, int32_t priority)
	{
		int32_t ret = -1;

		#if defined _WIN32 || _WIN64
			ret = SetThreadPriority(handle, priority) ? 0 : -1;
		#elif defined ANDROID
			struct sched_param sp;
			memset(&sp, 0, sizeof(struct sched_param));
			sp.sched_priority = priority;

			ret = pthread_setschedparam(handle, SCHED_RR, &sp);
		#endif

		return ret;
	}

	int32_t Thread::Destroy()
	{
		if (mThreadInfo != NULL && mThreadInfo->mState == Thread::FINISHED)
		{
			mAllocator.Deallocate(mThreadInfo);

			mThreadInfo = NULL;
			Detach();

			return 0;
		}
		else
			return -1;
	}

	int32_t Thread::Detach()
	{
		if (mThreadInfo != NULL && (mThreadInfo->mState == Thread::RUNNING || mThreadInfo->mState == Thread::FINISHED))
		{
			mThreadInfo = NULL;
			mHandle = NULL;

			return 0;
		}
		else
			return -1;
	}

	Thread::ThreadHandle Thread::_CreateThread(void* (*start) (void*), void* arg)
	{
		ThreadHandle handle = NULL;

		#if defined _WIN32 || _WIN64
			handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, arg, NULL, NULL);
		#elif defined ANDROID
			pthread_create(&handle, NULL, start, arg);
		#endif

		return handle;
	}

	void Thread::_JoinThread(ThreadHandle& handle)
	{
		#if defined _WIN32 || _WIN64
			if  (WaitForSingleObject(handle, INFINITE) == WAIT_FAILED)
			{
				CloseHandle(handle);
				handle = NULL;
			}
		#elif defined ANDROID
			pthread_join(handle, NULL);
		#endif
	}
}