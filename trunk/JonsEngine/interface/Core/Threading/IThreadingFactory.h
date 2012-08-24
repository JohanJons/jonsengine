#ifndef _JONS_ITHREADING_FACTORY_H
#define _JONS_ITHREADING_FACTORY_H

#include "interface/Core/Threading/IThread.h"
#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Threading/IConditionVariable.h"

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
	class IThreadingFactory
	{
	public:
		virtual ~IThreadingFactory() { }

		virtual IThread* const CreateThread() = 0;
		virtual IThread* const CreateThread(void* (*start) (void*), void* arg) = 0;
		virtual IMutex* const CreateMutex() = 0;
		virtual IConditionVariable* const CreateConditionVariable() = 0;

		virtual void DestroyThread(IThread* const thread) = 0;
		virtual void DestroyMutex(IMutex* const mutex) = 0;
		virtual void DestroyConditionVariable(IConditionVariable* const condVar) = 0;

	};

}

#endif