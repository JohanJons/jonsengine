#pragma once

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
	#include <time.h>
#endif

#include <stdint.h>

namespace JonsEngine
{
	class IMemoryAllocator;
	class Mutex;

	class ConditionVariable
	{
	public:
		enum ConditionVariableState
		{
			READY,
			WAITING
		};

		#if defined _WIN32 || _WIN64
			typedef CONDITION_VARIABLE ConditionVariableHandle;
		#else
			typedef pthread_cond_t ConditionVariableHandle;
		#endif

		ConditionVariable();
		~ConditionVariable();

		void Wait(Mutex& mutex);
		void TimedWait(Mutex& mutex, uint32_t milliseconds);
		void Signal();
		void Broadcast();

		const ConditionVariableState& GetConditionVariableState() const;

	private:
		ConditionVariableHandle mCondVarHandle;
		ConditionVariableState mCondVarState;
		
	};
}