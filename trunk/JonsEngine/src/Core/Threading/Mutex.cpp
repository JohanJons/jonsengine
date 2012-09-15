#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
#endif

#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	Mutex::Mutex(ILogManager& logger) : mLogger(logger), mState(Mutex::UNLOCKED)
	{
		#if defined _WIN32 || _WIN64
			InitializeCriticalSection(&mHandle);
		#else
			pthread_mutex_init(&mHandle, NULL);
		#endif
	}

	Mutex::~Mutex()
	{
		#if defined _WIN32 || _WIN64
			DeleteCriticalSection(&mHandle);
		#elif ANDROID
			pthread_mutex_destroy(&mHandle);
		#endif
	}

	int32_t Mutex::Lock()
	{
		int32_t ret = 0;

		#if defined _WIN32 || _WIN64
			EnterCriticalSection(&mHandle);
		#else
			ret = pthread_mutex_lock(&mHandle);
		#endif

		if (!ret)
			mState = Mutex::LOCKED;

		return ret;
	}

	int32_t Mutex::Unlock()
	{
		int32_t ret = 0;

		#if defined _WIN32 || _WIN64
			LeaveCriticalSection(&mHandle);
		#else
			ret = pthread_mutex_unlock(&mHandle);
		#endif

		if (!ret)
			mState = Mutex::UNLOCKED;

		return ret;
	}

	const Mutex::MutexState& Mutex::GetMutexState() const
	{
		return mState;
	}

	MutexHandle& Mutex::GetMutexHandle()
	{
		return mHandle;
	}
}