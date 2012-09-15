#include "../JonsEngineTests/Core/Threading/ThreadPoolTest.h"

#include "interface/Core/Threading/IThreadPool.h"

namespace JonsEngine
{

	/**
	 * Constructor test
	 */
	TEST_F(ThreadPoolTest, Constructor)
	{
		IThreadPool* pool = mEngine.GetThreadingFactory().CreateThreadPool(0);

		ASSERT_EQ(pool->Empty(), true);
		ASSERT_EQ(pool->GetNumThreads(), 0);
		ASSERT_EQ(pool->PendingTasks(), 0);

		IThreadPool* pool2 = mEngine.GetThreadingFactory().CreateThreadPool(30);

		ASSERT_EQ(pool2->Empty(), true);
		ASSERT_EQ(pool2->GetNumThreads(), 30);
		ASSERT_EQ(pool2->PendingTasks(), 0);

		mEngine.GetThreadingFactory().DestroyThreadPool(pool);
		mEngine.GetThreadingFactory().DestroyThreadPool(pool2);
	}

	/**
	 * AddTask test
	 */
	TEST_F(ThreadPoolTest, AddTask)
	{
		IThreadPool* pool = mEngine.GetThreadingFactory().CreateThreadPool(5);

		mEngine.GetThreadingFactory().DestroyThreadPool(pool);
	}

}