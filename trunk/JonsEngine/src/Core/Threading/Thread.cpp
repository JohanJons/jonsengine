#include "../../../include/Core/Threading/Thread.h"

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
	/*
	 *
	 * Thread wrapper class
	 *
	 */
	Thread::Thread() : mHandle(NULL), mAllocator(&SYSTEM_DEFAULT_HEAP_ALLOCATOR), mThreadInfo(NULL)
	{
	}

	Thread::Thread(void* (*start) (void*), void* arg, IMemoryAllocator* allocator) : mAllocator(allocator)
	{
		mThreadInfo = (ThreadInfo*) mAllocator->Allocate(sizeof(ThreadInfo));

		mThreadInfo->mArg = arg;
		mThreadInfo->mFunctionPointer = start;
		mThreadInfo->mState = Thread::RUNNING;

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
			thread->mFunctionPointer(thread->mArg);
			
			thread->mState = Thread::FINISHED;
		}

		return NULL;
	}

	Thread::ThreadHandle& Thread::GetNativeHandle()
	{
		return mHandle;
	}

	Thread::ThreadState Thread::GetThreadState() const
	{
		if (mThreadInfo != NULL)
			return mThreadInfo->mState;
		else
			return Thread::DETACHED;
	}

	int32_t Thread::Destroy()
	{
		if (mThreadInfo != NULL && mThreadInfo->mState == Thread::FINISHED)
		{
			mAllocator->Deallocate(mThreadInfo);

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
			mAllocator = NULL;
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


	/*
	 *
	 * Free-standing functions
	 *
	 */

	/*
	 * Sleeps the current thread of execution
	 * @param milliseconds: number of milliseconds to sleep
	 */
	void jons_SleepCurrentThread(uint32_t milliseconds)
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

	/*
	 * Returns the backend native thread handle
	 * @ret: The native thread backend for the current thread
	 */
	Thread::ThreadHandle jons_GetCurrentThreadNativeHandle()
	{
		Thread::ThreadHandle handle;

		#if defined _WIN32 || _WIN64
			handle = GetCurrentThread();
		#elif defined ANDROID
			handle = pthread_self();
		#endif

		return handle;
	}

	/*
	 * Sets the priority of the current thread
	 * @param priority: the priority to set this thread to
	 * @ret: 0 for success, -1 for fail
	 */
	int32_t jons_SetThreadPriority(int32_t priority)
	{
		int32_t ret = -1;

		#if defined _WIN32 || _WIN64
			ret = jons_SetThreadPriority(GetCurrentThread(), priority);
		#elif defined ANDROID
			ret = jons_SetThreadPriority(pthread_self(), priority);
		#endif

		return ret;
	}

	/*
	 * Sets the priority of a given thread
	 * @param handle: the native handle of the thread to set priority to
	 * @param priority: the priority to set to thread given by handle
	 * @ret: 0 for success, -1 for fail
	 */
	int32_t jons_SetThreadPriority(Thread::ThreadHandle handle, int32_t priority)
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
}