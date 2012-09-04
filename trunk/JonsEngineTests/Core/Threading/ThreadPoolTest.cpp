#include "../JonsEngineTests/Core/Threading/ThreadPoolTest.h"

#include "interface/Core/Threading/IThreadPool.h"

namespace JonsEngine
{

	/**
	 * Constructor test
	 */
	TEST_F(ThreadPoolTest, Constructor)
	{
		IThreadPool* pool = mEngine.GetThreadingFactory().CreateThreadPool(10);

		mEngine.GetThreadingFactory().DestroyThreadPool(pool);
	}

}