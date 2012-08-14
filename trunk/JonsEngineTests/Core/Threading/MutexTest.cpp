#include "MutexTest.h"


namespace JonsEngine
{
	int MutexTest::mCount = 0;

	/**
	 * Lock/Unlock test
	 */
	TEST_F(MutexTest, Lock)
	{
		Mutex mutex;
		for (int i = 0; i<10; i++)
			Thread thread(&incrementer, (void*)&mutex, mEngine.GetMemoryManager()->GetHeapAllocator());
		jons_SleepCurrentThread(500);

		ASSERT_EQ(10, MutexTest::mCount);
	}

	/**
	 * GetNativeHandle test
	 */
	TEST_F(MutexTest, GetNativeHandle)
	{
		Mutex mutex;
		
		Mutex::MutexHandle handle = mutex.GetNativeHandle();
	}

	/**
	 * GetMutexState test
	 */
	TEST_F(MutexTest, GetMutexState)
	{
		Mutex mutex;

		ASSERT_EQ(Mutex::UNLOCKED, mutex.GetMutexState());

		mutex.Lock();

		ASSERT_EQ(Mutex::LOCKED, mutex.GetMutexState());

		mutex.Unlock();

		ASSERT_EQ(Mutex::UNLOCKED, mutex.GetMutexState());
	}
}