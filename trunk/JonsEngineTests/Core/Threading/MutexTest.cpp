#include "../JonsEngineTests/Core/Threading/MutexTest.h"

#include "boost/bind.hpp"

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
			mEngine.GetThreadingFactory().CreateThread(boost::bind(&incrementer, mutex));
		jons_SleepCurrentThread(500);

		ASSERT_EQ(10, MutexTest::mCount);

		// Engine destructor will destroy allocated threads...
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