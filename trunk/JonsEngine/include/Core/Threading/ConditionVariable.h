#ifndef _JONS_CONDITION_VARIABLE_H
#define _JONS_CONDITION_VARIABLE_H

#ifdef ANDROID
#include <pthread.h>
#endif

#include "interface/Core/Threading/IConditionVariable.h"

namespace JonsEngine
{
	class ILogManager;

	class ConditionVariable : public IConditionVariable
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef void* ConditionVariableHandle;
		#else
			typedef pthread_cond_t ConditionVariableHandle;
		#endif

		ConditionVariable(ILogManager& logger);
		~ConditionVariable();

		void Wait();
		void TimedWait(uint32_t milliseconds);
		void Signal();
		void Broadcast();

		const ConditionVariableState& GetConditionVariableState() const;

	private:
		ConditionVariableHandle mCondVarHandle;
		ConditionVariableState mCondVarState;
		ILogManager& mLogger;
		#ifdef ANDROID
			Mutex mMutex;
		#endif
		
	};
}

#endif