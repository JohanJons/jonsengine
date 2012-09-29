#include "../JonsEngineTests/Core/Threading/ThreadPoolTest.h"

namespace JonsEngine
{

	/**
	 * Constructor test
	 */
	TEST_F(ThreadPoolTest, Constructor)
	{
		ThreadPool pool(0);

		ASSERT_EQ(pool.Empty(), true);
		ASSERT_EQ(pool.GetNumThreads(), 0);
		ASSERT_EQ(pool.PendingTasks(), 0);

		ThreadPool pool2(30);

		ASSERT_EQ(pool2.Empty(), true);
		ASSERT_EQ(pool2.GetNumThreads(), 30);
		ASSERT_EQ(pool2.PendingTasks(), 0);
	}

	/**
	 * AddTask test
	 */
	TEST_F(ThreadPoolTest, AddTask)
	{
		ThreadPool pool(5);

		AddTasks(pool, 10);

		ASSERT_NE(pool.PendingTasks(), 0);
		ASSERT_EQ(pool.GetNumThreads(), 5);
		ASSERT_EQ(pool.Empty(), false);
	}

	/**
	 * Cleartasks test
	 */
	TEST_F(ThreadPoolTest, Cleartasks)
	{
		ThreadPool pool(5);

		AddTasks(pool, 10);
		pool.ClearTasks();

		ASSERT_EQ(pool.PendingTasks(), 0);
		ASSERT_EQ(pool.Empty(), true);
	}

	/**
	 * Pendingtasks test
	 */
	TEST_F(ThreadPoolTest, Pendingtasks)
	{
		ThreadPool pool(5);

		AddTasks(pool, 10);

		ASSERT_NE(pool.PendingTasks(), 0);
		ASSERT_EQ(pool.Empty(), false);
	}

	/**
	 * Empty test
	 */
	TEST_F(ThreadPoolTest, Empty)
	{
		ThreadPool pool(5);

		ASSERT_EQ(pool.Empty(), true);

		AddTasks(pool, 10);

		ASSERT_EQ(pool.Empty(), false);

		pool.Wait();

		ASSERT_EQ(pool.Empty(), true);
	}

	/**
	 * Wait test
	 */
	TEST_F(ThreadPoolTest, Wait)
	{
		ThreadPool pool(5);

		AddTasks(pool, 10);

		ASSERT_NE(pool.PendingTasks(), 0);
		ASSERT_EQ(pool.GetNumThreads(), 5);
		ASSERT_EQ(pool.Empty(), false);

		pool.Wait();

		ASSERT_EQ(pool.Empty(), true);
		ASSERT_EQ(pool.GetNumThreads(), 5);
		ASSERT_EQ(pool.PendingTasks(), 0);
	}


	/**
	 * Wait(2) test
	 */
	TEST_F(ThreadPoolTest, Wait2)
	{
		ThreadPool pool(3);

		AddTasks(pool, 10);

		ASSERT_NE(pool.PendingTasks(), 0);
		ASSERT_EQ(pool.GetNumThreads(), 3);
		ASSERT_EQ(pool.Empty(), false);

		pool.Wait(5);

		ASSERT_LE((int)pool.PendingTasks(), 6);
	}

	/**
	 * SetNumThreads test
	 */
	TEST_F(ThreadPoolTest, SetNumThreads)
	{
		ThreadPool pool(5);

		ASSERT_EQ(pool.GetNumThreads(), 5);

		pool.SetNumThreads(7);

		ASSERT_EQ(pool.GetNumThreads(), 7);

		pool.SetNumThreads(3);

		SleepCurrentThread(100);

		ASSERT_EQ(pool.GetNumThreads(), 3);
	}

	/**
	 * GetNumThreads test
	 */
	TEST_F(ThreadPoolTest, GetNumThreads)
	{
		ThreadPool pool(5);

		ASSERT_EQ(pool.GetNumThreads(), 5);
	}

}