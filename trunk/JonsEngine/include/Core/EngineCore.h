#ifndef _ENGINE_CORE_H
#define _ENGINE_CORE_H

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/LogManagerImpl.h"
#include "include/Core/Memory/MemoryManagerImpl.h"


namespace JonsEngine
{
	/*
	 * The most vital parts of the engine, the logging and the memory management,
	 * which must be initialied first and destroyed last.
	 */
	class EngineCore
	{
	public:
		EngineCore() { }
		~EngineCore() { }

	protected:
		LogManagerImpl mLog;
		MemoryManagerImpl mMemoryManager;
	};

}

#endif
