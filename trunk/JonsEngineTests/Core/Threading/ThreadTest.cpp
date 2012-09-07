#include "../JonsEngineTests/Core/Threading/ThreadTest.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
	/**
	 * Thread jons_SleepCurrentThread test
	 */
	TEST_F(ThreadTest, jons_SleepCurrentThread)
	{
		int32_t num = 0;
		IThread* thread = mEngine.GetThreadingFactory().CreateThread(boost::bind(&ThreadTest::SetNumberTo14, this, &num));

		jons_SleepCurrentThread(200);

		ASSERT_EQ(num, 14);

		mEngine.GetThreadingFactory().DestroyThread(thread);
	}

	/**
	 * Thread jons_SetThreadPriority test
	 */
	TEST_F(ThreadTest, jons_SetThreadPriority)
	{
		ASSERT_EQ(0, jons_SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	}

	/**
	 * Thread constructor1 test
	 */
	TEST_F(ThreadTest, Constructor1)
	{
		IThread* thread = mEngine.GetThreadingFactory().CreateThread();
		
		ASSERT_EQ(Thread::DETACHED, thread->GetThreadState());
		ASSERT_EQ(-1, thread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL));

		ASSERT_EQ(-1, thread->Join());

		mEngine.GetThreadingFactory().DestroyThread(thread);
	}

	/**
	 * Thread constructor2 test
	 */
	TEST_F(ThreadTest, Constructor2)
	{
		int32_t arg = 500;
		IThread* thread = mEngine.GetThreadingFactory().CreateThread(boost::bind(&Sleeper, &arg));

		ASSERT_EQ(Thread::RUNNING, thread->GetThreadState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread->GetThreadState());

		mEngine.GetThreadingFactory().DestroyThread(thread);
	}

	/**
	 * Operator assignment test
	 */
	TEST_F(ThreadTest, operatorAssign)
	{
		int32_t arg = 500;
		IThread* tr1 = mEngine.GetThreadingFactory().CreateThread();
		IThread* tr2 = mEngine.GetThreadingFactory().CreateThread(boost::bind(&Sleeper, &arg));
		
		ASSERT_EQ(Thread::DETACHED, tr1->GetThreadState());
		ASSERT_EQ(Thread::RUNNING, tr2->GetThreadState());

		*tr1 = *tr2;

		ASSERT_EQ(Thread::RUNNING, tr1->GetThreadState());
		ASSERT_EQ(Thread::DETACHED, tr2->GetThreadState());

		mEngine.GetThreadingFactory().DestroyThread(tr1);
		mEngine.GetThreadingFactory().DestroyThread(tr2);
	}
	
	/**
	 * Thread Join test
	 */
	TEST_F(ThreadTest, Join)
	{
		int32_t arg = 500;
		IThread* thread = mEngine.GetThreadingFactory().CreateThread(boost::bind(&Sleeper, &arg));

		ASSERT_EQ(thread->GetThreadState(), Thread::RUNNING);

		thread->Join();

		ASSERT_EQ(thread->GetThreadState(), Thread::FINISHED);

		mEngine.GetThreadingFactory().DestroyThread(thread);
	}
	
	/**
	 * Thread SetPriority test
	 */
	TEST_F(ThreadTest, SetPriority)
	{
		int32_t arg = 500;
		IThread* thread = mEngine.GetThreadingFactory().CreateThread(boost::bind(&Sleeper, &arg));

		ASSERT_EQ(0, thread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL));

		mEngine.GetThreadingFactory().DestroyThread(thread);
	}

	/**
	 * Thread GetThreadState test
	 */
	TEST_F(ThreadTest, GetThreadState)
	{
		int32_t arg = 500;
		IThread* thread = mEngine.GetThreadingFactory().CreateThread(boost::bind(&Sleeper, &arg));

		ASSERT_EQ(Thread::RUNNING, thread->GetThreadState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread->GetThreadState());
		
		mEngine.GetThreadingFactory().DestroyThread(thread);
	}
}