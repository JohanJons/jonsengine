#include "ThreadTest.h"

#include "boost/bind.hpp"

#define THREAD_TEST_PRIO 1

namespace JonsEngine
{
	/**
	 * Thread SleepCurrentThread test
	 */
	TEST_F(ThreadTest, SleepCurrentThread)
	{
		int32_t num = 0;
		Thread thread(boost::bind(&ThreadTest::SetNumberTo14, this, &num));

		SleepCurrentThread(200);

		ASSERT_EQ(num, 14);
	}

	/**
	 * Thread constructor1 test
	 */
	TEST_F(ThreadTest, Constructor1)
	{
		Thread thread;
		
		ASSERT_EQ(Thread::DETACHED, thread.GetThreadState());
		ASSERT_EQ(-1, thread.SetPriority(THREAD_TEST_PRIO));

		ASSERT_EQ(-1, thread.Join());
	}

	/**
	 * Thread constructor2 test
	 */
	TEST_F(ThreadTest, Constructor2)
	{
		int32_t arg = 500;
		Thread thread(boost::bind(&ThreadTest::Sleeper, this, &arg));

		ASSERT_EQ(Thread::RUNNING, thread.GetThreadState());

		SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread.GetThreadState());
	}

	/**
	 * Operator assignment test
	 */
	TEST_F(ThreadTest, operatorAssign)
	{
		int32_t arg = 500;
		Thread thread1;
		Thread thread2(boost::bind(&ThreadTest::Sleeper, this, &arg));
		
		ASSERT_EQ(Thread::DETACHED, thread1.GetThreadState());
		ASSERT_EQ(Thread::RUNNING, thread2.GetThreadState());

		thread1 = thread2;

		ASSERT_EQ(Thread::RUNNING, thread1.GetThreadState());
		ASSERT_EQ(Thread::DETACHED, thread2.GetThreadState());
	}
	
	/**
	 * Thread Join test
	 */
	TEST_F(ThreadTest, Join)
	{
		int32_t arg = 500;
		Thread thread(boost::bind(&ThreadTest::Sleeper, this, &arg));

		ASSERT_EQ(thread.GetThreadState(), Thread::RUNNING);

		thread.Join();

		ASSERT_EQ(thread.GetThreadState(), Thread::FINISHED);
	}
	
	/**
	 * Thread SetPriority test
	 */
	TEST_F(ThreadTest, SetPriority)
	{
		int32_t arg = 500;
		Thread thread(boost::bind(&ThreadTest::Sleeper, this, &arg));

		ASSERT_EQ(0, thread.SetPriority(THREAD_TEST_PRIO));
	}

	/**
	 * Thread GetThreadState test
	 */
	TEST_F(ThreadTest, GetThreadState)
	{
		int32_t arg = 500;
		Thread thread(boost::bind(&ThreadTest::Sleeper, this, &arg));

		ASSERT_EQ(Thread::RUNNING, thread.GetThreadState());

		SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread.GetThreadState());
	}
}