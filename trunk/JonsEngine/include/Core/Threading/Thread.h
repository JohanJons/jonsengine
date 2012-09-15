#ifndef _JONS_THREAD_H
#define _JONS_THREAD_H

#ifdef ANDROID
#include <pthread.h>
#endif

#include "interface/Core/Threading/IThread.h"

namespace JonsEngine
{
	class IMemoryAllocator;
	class ILogManager;

	#if defined _WIN32 || _WIN64
		typedef void* ThreadHandle;  
	#else
		typedef pthread_t ThreadHandle;
	#endif

	/* Thread wrapper class */
	class Thread : public IThread
	{
	public:
		struct ThreadInfo
		{
			ThreadInfo() : mState(DETACHED), mTask(NULL)
			{ }

			ThreadState mState;
			Task mTask;
		};

		Thread(IMemoryAllocator& allocator, ILogManager& logger);
		Thread(Task task, IMemoryAllocator& allocator, ILogManager& logger);
		~Thread();

		Thread& operator=(Thread& other);
		int32_t Join();
		int32_t SetPriority(int32_t priority);

		ThreadState GetThreadState() const;

	private:
		ThreadHandle mHandle;
		ThreadInfo* mThreadInfo;
		IMemoryAllocator& mAllocator;
		ILogManager& mLogger;

		int32_t Destroy();
		int32_t Detach();
		static void* Run(void* arg);
		ThreadHandle _CreateThread(void* (*start) (void*), void* arg);
		void _JoinThread(ThreadHandle& handle);
	};

	ThreadHandle jons_GetCurrentThreadNativeHandle();
	int32_t jons_SetThreadPriority(ThreadHandle handle, int32_t priority);
}

#endif