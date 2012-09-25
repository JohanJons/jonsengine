#include "include/Core/Threading/ConditionVariable.h"

#include "interface/Core/Logging/ILogger.h"

#include "include/Core/Threading/Mutex.h"


namespace JonsEngine
{
	ConditionVariable::ConditionVariable() : mLogger(Globals::GetDefaultLogger()), mCondVarState(ConditionVariable::READY)
	{
		#if defined _WIN32 || _WIN64
			InitializeConditionVariable(&mCondVarHandle);
		#else
			pthread_cond_init(&mCondVarHandle, NULL);
		#endif
	}

	ConditionVariable::~ConditionVariable()
	{
		#ifdef ANDROID
			pthread_cond_destroy(&mCondVarHandle);
		#endif
	}

	void ConditionVariable::Wait(Mutex& mutex)
	{
		Mutex::MutexHandle& mutexHandle = mutex.GetMutexHandle();

		mCondVarState = ConditionVariable::WAITING;

		#if defined _WIN32 || _WIN64
			SleepConditionVariableCS(&mCondVarHandle, &mutexHandle, INFINITE);
		#else
			pthread_cond_wait(&mCondVarHandle, mutexHandle);
		#endif

		mCondVarState = ConditionVariable::READY;
	}

	void ConditionVariable::TimedWait(Mutex& mutex, uint32_t milliseconds)
	{
		Mutex::MutexHandle& mutexHandle = mutex.GetMutexHandle();

		mCondVarState = ConditionVariable::WAITING;

		#if defined _WIN32 || _WIN64
			SleepConditionVariableCS(&mCondVarHandle, &mutexHandle, milliseconds);
		#else
			struct timespec ts;
			struct timeval tp;

			gettimeofday(&tp, NULL);

			ts.tv_sec  = tp.tv_sec;
			ts.tv_nsec = tp.tv_usec * 1000;
			ts.tv_sec += milliseconds/1000;

			pthread_cond_timedwait(&mCondVarHandle, &mutexHandle, &ts);
		#endif

		mCondVarState = ConditionVariable::READY;
	}

	void ConditionVariable::Signal()
	{
		#if defined _WIN32 || _WIN64
			WakeConditionVariable(&mCondVarHandle);
		#else
			pthread_cond_signal(&mCondVarHandle->mHandle);
		#endif
	}

	void ConditionVariable::Broadcast()
	{
		#if defined _WIN32 || _WIN64
			WakeAllConditionVariable(&mCondVarHandle);
		#else
			pthread_cond_broadcast(&mCondVarHandle);
		#endif
	}

	const ConditionVariable::ConditionVariableState& ConditionVariable::GetConditionVariableState() const
	{
		return mCondVarState;
	}
}