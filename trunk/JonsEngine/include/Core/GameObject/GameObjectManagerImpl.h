#ifndef _OBJECT_MANAGER_IMPL_H
#define _OBJECT_MANAGER_IMPL_H

#include <map>

#include "interface/Core/GameObject/IGameObjectManager.h"
#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Memory/IMemoryManager.h"

#include "include/Core/EngineDefs.h"
#include "include/Core/GameObject/GameObject.h"

namespace JonsEngine
{
	class GameObjectManagerImpl : public IGameObjectManager
	{
	public:
		GameObjectManagerImpl(ILogManager& logger, IMemoryAllocator* memAllocator);
		~GameObjectManagerImpl();

		bool Init();
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();

	private:
		ILogManager& mLog;
		bool mRunning;
		bool mInitialized;
	};


}

#endif