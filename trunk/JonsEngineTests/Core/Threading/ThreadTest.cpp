#include "../JonsEngineTests/Core/Threading/ThreadTest.h"


namespace JonsEngine
{
	/**
	 * Thread jons_SleepCurrentThread test
	 */
	TEST_F(ThreadTest, jons_SleepCurrentThread)
	{
		int num = 0;
		Thread thread(&SetNumberTo14, (void*)&num, mEngine.GetMemoryManager()->GetHeapAllocator());

		jons_SleepCurrentThread(200);

		ASSERT_EQ(num, 14);
	}

	/**
	 * Thread jons_GetCurrentThreadNativeHandle test
	 */
	TEST_F(ThreadTest, jons_GetCurrentThreadNativeHandle)
	{
		jons_GetCurrentThreadNativeHandle();
	}

	/**
	 * Thread jons_SetThreadPriority test
	 */
	TEST_F(ThreadTest, jons_SetThreadPriority)
	{
		ASSERT_EQ(0, jons_SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	}

	/**
	 * Thread jons_SetThreadPriority2 test
	 */
	TEST_F(ThreadTest, jons_SetThreadPriority2)
	{
		Thread thread(&Sleeper, (void*)1000, mEngine.GetMemoryManager()->GetHeapAllocator());

		ASSERT_EQ(0, jons_SetThreadPriority(thread.GetNativeHandle(), THREAD_PRIORITY_ABOVE_NORMAL));
	}

	/**
	 * Thread constructor1 test
	 */
	TEST_F(ThreadTest, Constructor1)
	{
		Thread* thread = new Thread();
		
		ASSERT_EQ(Thread::DETACHED, thread->GetThreadState());
		ASSERT_EQ(NULL, thread->GetNativeHandle());
		ASSERT_EQ(-1, thread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL));

		ASSERT_EQ(-1, thread->Join());

		delete thread;
	}

	/**
	 * Thread constructor2 test
	 */
	TEST_F(ThreadTest, Constructor2)
	{
		Thread* thread = new Thread(&Sleeper, (void*)500, mEngine.GetMemoryManager()->GetHeapAllocator());

		ASSERT_EQ(Thread::RUNNING, thread->GetThreadState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread->GetThreadState());

		delete thread;
	}

	/**
	 * Operator assignment test
	 */
	TEST_F(ThreadTest, operatorAssign)
	{
		Thread* tr1 = new Thread();
		Thread* tr2 = new Thread(&Sleeper, (void*)500, mEngine.GetMemoryManager()->GetHeapAllocator());
		
		ASSERT_EQ(Thread::DETACHED, tr1->GetThreadState());
		ASSERT_EQ(Thread::RUNNING, tr2->GetThreadState());

		*tr1 = *tr2;

		ASSERT_EQ(Thread::RUNNING, tr1->GetThreadState());
		ASSERT_EQ(Thread::DETACHED, tr2->GetThreadState());

		delete tr2;
		delete tr1;
	}
	
	/**
	 * Thread Join test
	 */
	TEST_F(ThreadTest, Join)
	{
		Thread* thread = new Thread(&Sleeper, (void*)1000, mEngine.GetMemoryManager()->GetHeapAllocator());

		ASSERT_EQ(thread->GetThreadState(), Thread::RUNNING);

		thread->Join();

		ASSERT_EQ(thread->GetThreadState(), Thread::FINISHED);

		delete thread;
	}
	
	/**
	 * Thread SetPriority test
	 */
	TEST_F(ThreadTest, SetPriority)
	{
		Thread* thread = new Thread(&Sleeper, (void*)500, mEngine.GetMemoryManager()->GetHeapAllocator());

		ASSERT_EQ(0, thread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL));

		delete thread;
	}

	/**
	 * Thread GetNativeHandle test
	 */
	TEST_F(ThreadTest, GetNativeHandle)
	{
		Thread* thread = new Thread(&Sleeper, (void*)500, mEngine.GetMemoryManager()->GetHeapAllocator());

		Thread::ThreadHandle handle = thread->GetNativeHandle();

		ASSERT_EQ(NULL, jons_SetThreadPriority(handle, THREAD_PRIORITY_ABOVE_NORMAL));

		delete thread;
	}

	/**
	 * Thread GetThreadState test
	 */
	TEST_F(ThreadTest, GetThreadState)
	{
		Thread* thread = new Thread(&Sleeper, (void*)500, mEngine.GetMemoryManager()->GetHeapAllocator());

		ASSERT_EQ(Thread::RUNNING, thread->GetThreadState());

		jons_SleepCurrentThread(1000);

		ASSERT_EQ(Thread::FINISHED, thread->GetThreadState());
		
		delete thread;
	}
}