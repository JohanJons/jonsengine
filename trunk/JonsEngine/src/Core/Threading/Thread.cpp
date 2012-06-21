#include "../../../include/Core/Threading/Thread.h"


namespace JonsEngine
{
	ThreadHandle SpawnThread(void* (*start) (void*), void* arg)
	{
		ThreadHandle handle = NULL;

		#if defined _WIN32 || _WIN64
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, arg, NULL, NULL);
		#elif defined ANDROID
			pthread_create(&handle, NULL, start, arg);
		#endif

		return handle;
	}

	ThreadHandle SpawnThread(void* (*start) (void*), void* arg, bool suspended)
	{
		ThreadHandle handle = NULL;

		#if defined _WIN32 || _WIN64
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, arg, suspended ? CREATE_SUSPENDED : NULL, NULL);
		#elif defined ANDROID
			pthread_create(&handle, NULL, start, arg);
		#endif

		return handle;
	}

	void SleepThread(uint64_t milliseconds)
	{

	}

	void JoinThread()
	{

	}

	void JoinThread(uint64_t timeoutMilliseconds)
	{

	}

	void SetThreadPriority(int32_t priority)
	{

	}

	void SetThreadPriority(ThreadHandle handle, int32_t priority)
	{

	}

	void DestroyThread(ThreadHandle handle)
	{

	}

}

