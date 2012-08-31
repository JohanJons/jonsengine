#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
	#include <time.h>
#endif

#include "include/Core/Threading/ConditionVariable.h"

namespace JonsEngine
{
	ConditionVariable::ConditionVariable(ILogManager& logger) : mLogger(logger), mCondVarState(ConditionVariable::READY)
	{
		#if defined _WIN32 || _WIN64
			mCondVarHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
		#else
			pthread_cond_init(&mCondVarHandle, NULL);
		#endif
	}

	ConditionVariable::~ConditionVariable()
	{
		#if defined _WIN32 || _WIN64
			CloseHandle(mCondVarHandle);
		#else
			pthread_cond_destroy(&mCondVarHandle);
		#endif
	}

	void ConditionVariable::Wait()
	{
		mCondVarState = ConditionVariable::WAITING;

		#if defined _WIN32 || _WIN64
			WaitForSingleObject(mCondVarHandle, INFINITE);
		#else
			mMutex.Lock();

			pthread_cond_wait(&mCondVarHandle, &mMutex.GetNativeHandle());

			mMutex.Unlock();
		#endif

		mCondVarState = ConditionVariable::READY;
	}

	void ConditionVariable::TimedWait(uint32_t milliseconds)
	{
		mCondVarState = ConditionVariable::WAITING;

		#if defined _WIN32 || _WIN64
			WaitForSingleObject(mCondVarHandle, milliseconds);
		#else
			struct timespec ts;
			struct timeval tp;

			mMutex.Lock();

			gettimeofday(&tp, NULL);

			ts.tv_sec  = tp.tv_sec;
			ts.tv_nsec = tp.tv_usec * 1000;
			ts.tv_sec += milliseconds/1000;

			pthread_cond_timedwait(&mCondVarHandle, &mMutex.GetNativeHandle(), &ts);

			mMutex.Unlock();
		#endif

		mCondVarState = ConditionVariable::READY;
	}

	void ConditionVariable::Signal()
	{
		#if defined _WIN32 || _WIN64
			SetEvent(mCondVarHandle);

			ResetEvent(mCondVarHandle);
		#else
			mMutex.Lock();

			pthread_cond_signal(&mCondVarHandle);

			mMutex.Unlock();
		#endif
	}

	void ConditionVariable::Broadcast()
	{
		#if defined _WIN32 || _WIN64
			SetEvent(mCondVarHandle);

			ResetEvent(mCondVarHandle);
		#else
			mMutex.Lock();

			pthread_cond_broadcast(&mCondVarHandle);

			mMutex.Unlock();
		#endif
	}

	const ConditionVariable::ConditionVariableState& ConditionVariable::GetConditionVariableState() const
	{
		return mCondVarState;
	}
}