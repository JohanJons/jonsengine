#include "../JonsEngineTests/Core/Threading/ConditionVariableTest.h"

#include "boost/bind.hpp"

namespace JonsEngine
{

	int ConditionVariableTest::mCount = 0;
	IMutex* ConditionVariableTest::mMutex = NULL;
	IConditionVariable* ConditionVariableTest::mCondVar = NULL;

	/**
	 * Wait/Broadcast test
	 */
	TEST_F(ConditionVariableTest, Wait)
	{
		mMutex = mEngine.GetThreadingFactory().CreateMutex();
		mCondVar = mEngine.GetThreadingFactory().CreateConditionVariable();
		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(&Incrementto4);
		IThread* tr2 = mEngine.GetThreadingFactory().CreateThread(&Incrementto7);
		IThread* tr3 = mEngine.GetThreadingFactory().CreateThread(&Incrementto10);

		for (;;)
		{
			mCondVar->Wait();

			if (tr1->GetThreadState() == Thread::FINISHED &&
				tr2->GetThreadState() == Thread::FINISHED &&
				tr3->GetThreadState() == Thread::FINISHED)
				break;
		}

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyThread(tr2);
		mEngine.GetThreadingFactory().DestroyThread(tr3);
		mEngine.GetThreadingFactory().DestroyMutex(mMutex);
		mEngine.GetThreadingFactory().DestroyConditionVariable(mCondVar);
	}

	/**
	 * TimedWait/signal test
	 */
	TEST_F(ConditionVariableTest, TimedWait)
	{
		mCondVar = mEngine.GetThreadingFactory().CreateConditionVariable();
		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(&signal);

		mCondVar->TimedWait(1000);

		ASSERT_EQ(mCount, 10);

		IThread* tr2 = mEngine.GetThreadingFactory().CreateThread(&setCountTo14);

		mCondVar->TimedWait(500);

		ASSERT_EQ(mCount, 14);

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyThread(tr2);
		mEngine.GetThreadingFactory().DestroyConditionVariable(mCondVar);
	}

	/**
	 * GetConditionVariableState test
	 */
	TEST_F(ConditionVariableTest, GetConditionVariableState)
	{
		mCondVar = mEngine.GetThreadingFactory().CreateConditionVariable();

		ASSERT_EQ(ConditionVariable::READY, mCondVar->GetConditionVariableState());

		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread(&timedWait500ms);

		jons_SleepCurrentThread(100);

		ASSERT_EQ(ConditionVariable::WAITING, mCondVar->GetConditionVariableState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(ConditionVariable::READY, mCondVar->GetConditionVariableState());

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyConditionVariable(mCondVar);
	}
}