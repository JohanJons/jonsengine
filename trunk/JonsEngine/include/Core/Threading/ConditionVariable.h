#ifndef _JONS_CONDITION_VARIABLE_H
#define _JONS_CONDITION_VARIABLE_H

#include "interface/Core/Threading/IConditionVariable.h"

namespace JonsEngine
{
	class ConditionVariable : public IConditionVariable
	{
	public:
		ConditionVariable(ILogManager& logger);
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
		ILogManager& mLogger;
		#if defined __GNUG__
			Mutex mMutex;
		#endif
		
	};
}

#endif