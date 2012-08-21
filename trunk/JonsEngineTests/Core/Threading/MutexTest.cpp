#include "../JonsEngineTests/Core/Threading/MutexTest.h"


namespace JonsEngine
{
	int MutexTest::mCount = 0;

	/**
	 * Lock/Unlock test
	 */
	TEST_F(MutexTest, Lock)
	{
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();

		for (int i = 0; i<10; i++)
			mEngine.GetThreadingFactory().CreateThread(&incrementer, (void*)mutex);
		jons_SleepCurrentThread(500);

		ASSERT_EQ(10, MutexTest::mCount);

		// Engine destructor will destroy allocated threads...
	}

	/**
	 * GetNativeHandle test
	 */
	TEST_F(MutexTest, GetNativeHandle)
	{
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();
		
		Mutex::MutexHandle handle = mutex->GetNativeHandle();

		mEngine.GetThreadingFactory().DestroyMutex(mutex);
	}

	/**
	 * GetMutexState test
	 */
	TEST_F(MutexTest, GetMutexState)
	{
		IMutex* mutex = mEngine.GetThreadingFactory().CreateMutex();

		ASSERT_EQ(Mutex::UNLOCKED, mutex->GetMutexState());

		mutex->Lock();

		ASSERT_EQ(Mutex::LOCKED, mutex->GetMutexState());

		mutex->Unlock();

		ASSERT_EQ(Mutex::UNLOCKED, mutex->GetMutexState());
	}
}