#ifndef _JONS_CONDITION_VARIABLE_H
#define _JONS_CONDITION_VARIABLE_H

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
	#include <time.h>
#endif

#include "include/Core/EngineDefs.h"
#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	class ConditionVariable
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef HANDLE ConditionVariableHandle;
		#else
			typedef pthread_cond_t ConditionVariableHandle;
		#endif

		enum ConditionVariableState
		{
			READY,
			WAITING
		};

		ConditionVariable();
		~ConditionVariable();

		void Wait();
		void TimedWait(uint32_t milliseconds);
		void Signal();
		void Broadcast();

		ConditionVariableHandle& GetNativeConditionVariableHandle();
		const ConditionVariableState& GetConditionVariableState() const;

	private:
		ConditionVariableHandle mCondVarHandle;
		ConditionVariableState mCondVarState;
		#if defined __GNUG__
			Mutex mMutex;
		#endif
		
	};
}

#endif