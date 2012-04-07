#ifndef _TEST_MANAGER_H
#define _TEST_MANAGER_H

#include "Core/Memory/MemoryTestManager.h"
#include "Core/GameObject/GameObjectTestManager.h"
#include "Core/Engine.h"

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
		EngineSettings mEngineSettings;
		ILogManager* mLogger;

		MemoryTestManager* mMemTestMgr;
		GameObjectTestManager* mGameObjTestMgr;

		uint64_t mRepeatTimes;

	};

}


#endif
