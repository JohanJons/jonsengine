#ifndef _JONS_CONDITION_VARIABLE_H
#define _JONS_CONDITION_VARIABLE_H

#include "interface/Core/Threading/IConditionVariable.h"

#if defined _WIN32 || _WIN64
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#elif ANDROID
#include <pthread.h>
#endif

namespace JonsEngine
{
	#if defined _WIN32 || _WIN64
		typedef CONDITION_VARIABLE ConditionVariableHandle;
	#else
		typedef pthread_cond_t ConditionVariableHandle;
	#endif

	class ILogManager;

	class ConditionVariable : public IConditionVariable
	{
	public:
		ConditionVariable(ILogManager& logger);
		~ConditionVariable();

		void Wait(IMutex* mutex);
		void TimedWait(IMutex* mutex, uint32_t milliseconds);
		void Signal();
		void Broadcast();

		const ConditionVariableState& GetConditionVariableState() const;

	private:
		ConditionVariableHandle mCondVarHandle;
		ConditionVariableState mCondVarState;
		ILogManager& mLogger;
		
	};
}

#endif