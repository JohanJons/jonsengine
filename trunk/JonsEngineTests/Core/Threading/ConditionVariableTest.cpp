#include "../JonsEngineTests/Core/Threading/ConditionVariableTest.h"

#include "boost/bind.hpp"

namespace JonsEngine
{


	/**
	 * Wait/Broadcast test
	 */
	TEST_F(ConditionVariableTest, Wait)
	{
		int32_t count = 0;
		bool waiting = true;
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();
		IConditionVariable* condVar = mEngine.GetThreadingFactory().CreateConditionVariable();
		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(boost::bind(&ConditionVariableTest::Increment, this, &count, &waiting, mutex, condVar));
		IThread* tr2 = mEngine.GetThreadingFactory().CreateThread(boost::bind(&ConditionVariableTest::Increment, this, &count, &waiting, mutex, condVar));

		{
			jons_SleepCurrentThread(10);

			ScopedLock lock(mutex);

			waiting = false;

			condVar->Broadcast();
		}

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyThread(tr2);
		mEngine.GetThreadingFactory().DestroyMutex(mutex);
		mEngine.GetThreadingFactory().DestroyConditionVariable(condVar);
	}

	/**
	 * TimedWait/signal test
	 */
	TEST_F(ConditionVariableTest, TimedWait)
	{
		int32_t count = 0;
		bool waiting = true;
		IConditionVariable* condVar = mEngine.GetThreadingFactory().CreateConditionVariable();
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();
		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(boost::bind(&ConditionVariableTest::Signal, this, &count, &waiting, mutex, condVar));

		{
			ScopedLock lock(mutex);

			while (waiting)
				condVar->TimedWait(mutex, 1000);

			ASSERT_EQ(count, 1);

			condVar->TimedWait(mutex, 500);
		}

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyMutex(mutex);
		mEngine.GetThreadingFactory().DestroyConditionVariable(condVar);
	}

	/**
	 * GetConditionVariableState test
	 */
	TEST_F(ConditionVariableTest, GetConditionVariableState)
	{
		IConditionVariable* condVar = mEngine.GetThreadingFactory().CreateConditionVariable();
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();

		ASSERT_EQ(ConditionVariable::READY, condVar->GetConditionVariableState());

		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(boost::bind(&ConditionVariableTest::timedWait100ms, this, mutex, condVar));

		jons_SleepCurrentThread(20);

		ASSERT_EQ(ConditionVariable::WAITING, condVar->GetConditionVariableState());

		jons_SleepCurrentThread(200);

		ASSERT_EQ(ConditionVariable::READY, condVar->GetConditionVariableState());

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyMutex(mutex);
		mEngine.GetThreadingFactory().DestroyConditionVariable(condVar);
	}
}