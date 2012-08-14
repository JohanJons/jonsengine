#include "../../../include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	Mutex::Mutex()
	{
		#if defined _WIN32 || _WIN64
			mHandle = CreateMutex(NULL, FALSE, NULL);
		#else
			pthread_mutex_init(&mHandle, NULL);
		#endif

		mState = Mutex::UNLOCKED;
	}

	Mutex::~Mutex()
	{
		#if defined _WIN32 || _WIN64
			CloseHandle(mHandle);
		#else
			pthread_mutex_destroy(&mHandle);
		#endif
	}

	int32_t Mutex::Lock()
	{
		int32_t ret = 0;

		#if defined _WIN32 || _WIN64
			ret = WaitForSingleObject(mHandle, INFINITE);
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
			ret = ReleaseMutex(mHandle) ? 0 : -1;
		#else
			ret = pthread_mutex_unlock(&mHandle);
		#endif

		if (!ret)
			mState = Mutex::UNLOCKED;

		return ret;
	}

	Mutex::MutexHandle& Mutex::GetNativeHandle()
	{
		return mHandle;
	}

	const Mutex::MutexState& Mutex::GetMutexState() const
	{
		return mState;
	}
}