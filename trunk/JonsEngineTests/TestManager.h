#ifndef _TEST_MANAGER_H
#define _TEST_MANAGER_H

#include "include/Core/Engine.h"

#include "../JonsEngineTests/Core/Memory/MemoryTestManager.h"
#include "../JonsEngineTests/Core/GameObject/GameObjectTestManager.h"

namespace JonsEngine
{

	class TestManager 
	{
	public:
		TestManager();
		TestManager(uint64_t RepeatTimes);
		~TestManager();

		bool RunAllTests();

		bool RunMemoryTests();
		bool RunGameObjectTests();

	private:
		void Init(uint64_t RepeatTimes);
		bool InitEngine();
		bool StartEngine();
		bool StopEngine();
		bool DestroyEngine();

		Engine* mEngine;
		ILogManager* mLogger;

		MemoryTestManager* mMemTestMgr;
		GameObjectTestManager* mGameObjTestMgr;

		uint64_t mRepeatTimes;

	};

}


#endif
