#include "../JonsEngineTests/Core/Threading/MutexTest.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
	/**
	 * Lock/Unlock test
	 */
	TEST_F(MutexTest, Lock)
	{
		Mutex mutex;
		int count = 0;

		Thread thread1(boost::bind(&incrementer, &mutex, &count));
		Thread thread2(boost::bind(&incrementer, &mutex, &count));
		Thread thread3(boost::bind(&incrementer, &mutex, &count));
		Thread thread4(boost::bind(&incrementer, &mutex, &count));

		SleepCurrentThread(500);

		ASSERT_EQ(4, count);
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