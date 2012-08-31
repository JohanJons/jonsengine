#ifndef _JONS_GAMEOBJECT_H
#define _JONS_GAMEOBJECT_H

#include "interface/Core/Memory/IMemoryManager.h"
#include "interface/Core/EngineDefs.h"

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