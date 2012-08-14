#include "ConditionVariableTest.h"


namespace JonsEngine
{

	int ConditionVariableTest::mCount = 0;
	Mutex ConditionVariableTest::mMutex;
	ConditionVariable ConditionVariableTest::mCondVar;

	/**
	 * Wait/Broadcast test
	 */
	TEST_F(ConditionVariableTest, Wait)
	{
		Thread tr1(&Incrementto4, NULL, mEngine.GetMemoryManager()->GetHeapAllocator()), 
				tr2(&Incrementto7, NULL, mEngine.GetMemoryManager()->GetHeapAllocator()),
				tr3(&Incrementto10, NULL, mEngine.GetMemoryManager()->GetHeapAllocator());

		for (;;)
		{
			mCondVar.Wait();

			if (tr1.GetThreadState() == Thread::FINISHED &&
				tr2.GetThreadState() == Thread::FINISHED &&
				tr3.GetThreadState() == Thread::FINISHED)
				break;
		}
	}

	/**
	 * TimedWait/signal test
	 */
	TEST_F(ConditionVariableTest, TimedWait)
	{
		Thread tr1(&signal, NULL, mEngine.GetMemoryManager()->GetHeapAllocator());

		mCondVar.TimedWait(1000);

		ASSERT_EQ(mCount, 10);

		Thread tr2(&setCountTo14, NULL, mEngine.GetMemoryManager()->GetHeapAllocator());

		mCondVar.TimedWait(500);

		ASSERT_EQ(mCount, 14);
	}

	/**
	 * GetNativeConditionVariableHandle test
	 */
	TEST_F(ConditionVariableTest, GetNativeConditionVariableHandle)
	{
		ConditionVariable::ConditionVariableHandle handle = mCondVar.GetNativeConditionVariableHandle();
	}

	/**
	 * GetConditionVariableState test
	 */
	TEST_F(ConditionVariableTest, GetConditionVariableState)
	{
		ASSERT_EQ(ConditionVariable::READY, mCondVar.GetConditionVariableState());

		Thread tr1(&timedWait500ms, NULL, mEngine.GetMemoryManager()->GetHeapAllocator());

		jons_SleepCurrentThread(100);

		ASSERT_EQ(ConditionVariable::WAITING, mCondVar.GetConditionVariableState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(ConditionVariable::READY, mCondVar.GetConditionVariableState());
	}
}