#include "ConditionVariableTest.h"

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
		Mutex mutex;
		ConditionVariable condVar;
		Thread thread1(boost::bind(&ConditionVariableTest::Increment, this, &count, &waiting, &mutex, &condVar));
		Thread thread2(boost::bind(&ConditionVariableTest::Increment, this, &count, &waiting, &mutex, &condVar));

		{
			SleepCurrentThread(10);

			ScopedLock lock(mutex);

			waiting = false;

			condVar.Broadcast();
		}
	}

	/**
	 * TimedWait/signal test
	 */
	TEST_F(ConditionVariableTest, TimedWait)
	{
		int32_t count = 0;
		bool waiting = true;
		ConditionVariable condVar;
		Mutex mutex;
		Thread tr1(boost::bind(&ConditionVariableTest::Signal, this, &count, &waiting, &mutex, &condVar));

		{
			ScopedLock lock(mutex);

			while (waiting)
				condVar.TimedWait(mutex, 1000);

			ASSERT_EQ(count, 1);

			condVar.TimedWait(mutex, 500);
		}
	}

	/**
	 * GetConditionVariableState test
	 */
	TEST_F(ConditionVariableTest, GetConditionVariableState)
	{
		ConditionVariable condVar;
		Mutex mutex;

		ASSERT_EQ(ConditionVariable::READY, condVar.GetConditionVariableState());

		Thread tr1(boost::bind(&ConditionVariableTest::timedWait100ms, this, &mutex, &condVar));

		SleepCurrentThread(20);

		ASSERT_EQ(ConditionVariable::WAITING, condVar.GetConditionVariableState());

		SleepCurrentThread(200);

		ASSERT_EQ(ConditionVariable::READY, condVar.GetConditionVariableState());
	}
}