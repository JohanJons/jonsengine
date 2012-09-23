#pragma once

#ifdef ANDROID
#include <pthread.h>
#endif

#include "boost/function.hpp"

namespace JonsEngine
{
	class IMemoryAllocator;
	class Logger;

	/* Free-standing function */
	void jons_SleepCurrentThread(uint32_t milliseconds);

	/* Task definition */
	typedef boost::function0<void> Task;

	/* Thread wrapper class */
	class Thread
	{
	public:
		enum ThreadState
		{
			DETACHED = 0,
			RUNNING,
			FINISHED
		};

		#if defined _WIN32 || _WIN64
			typedef void* ThreadHandle;  
		#else
			typedef pthread_t ThreadHandle;
		#endif

		struct ThreadInfo
		{
			ThreadInfo() : mState(DETACHED), mTask(NULL)
			{ }

			ThreadState mState;
			Task mTask;
		};

		Thread();
		Thread(Task task);
		~Thread();

		Thread& operator=(Thread& other);
		int32_t Join();
		int32_t SetPriority(int32_t priority);

		ThreadState GetThreadState() const;

	private:
		int32_t jons_SetThreadPriority(ThreadHandle handle, int32_t priority);
		int32_t Destroy();
		int32_t Detach();
		static void* Run(void* arg);
		ThreadHandle _CreateThread(void* (*start) (void*), void* arg);
		void _JoinThread(ThreadHandle& handle);

		ThreadHandle mHandle;
		ThreadInfo* mThreadInfo;
		IMemoryAllocator& mAllocator;
		Logger& mLogger;
	};

	
}