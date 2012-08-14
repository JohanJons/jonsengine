#ifndef _JONS_GAMEOBJECT_H
#define _JONS_GAMEOBJECT_H

#include "interface/Core/EngineDefs.h"
#include "interface/Core/Memory/IMemoryManager.h"


namespace JonsEngine
{
	class GameObject
	{
	friend class IGameObjectManager;

	public:
		virtual ~GameObject() { }

	private:
		uint64_t GameObjectID;

	};

}

#endif