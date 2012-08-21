#ifndef _JONS_THREADING_FACTORY_H
#define _JONS_THREADING_FACTORY_H

#include "interface/Core/Threading/IThreadingFactory.h"
#include "interface/Core/Memory/IMemoryAllocator.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/Mutex.h"
#include "include/Core/Threading/ConditionVariable.h"

#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
	class ThreadingFactory : public IThreadingFactory
	{
	public:
		ThreadingFactory(ILogManager& logger, IMemoryAllocator& memAllocator);
		~ThreadingFactory();

		bool Init();
		bool Destroy();

		IThread* const CreateThread();
		IThread* const CreateThread(void* (*start) (void*), void* arg);
		IMutex* const CreateMutex();
		IConditionVariable* const CreateConditionVariable();

		void DestroyThread(IThread* const thread);
		void DestroyMutex(IMutex* const mutex);
		void DestroyConditionVariable(IConditionVariable* const condVar);

	private:
		IMemoryAllocator& mMemoryAllocator;
		ILogManager& mLogger;
		Vector<IThread*> mCreatedThreads;
		Vector<IMutex*> mCreatedMutexes;
		Vector<IConditionVariable*> mCreatedCondVars;
	};

}

#endif