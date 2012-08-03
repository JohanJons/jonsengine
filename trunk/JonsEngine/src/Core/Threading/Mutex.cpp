#include "../../../include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	Mutex::Mutex()
	{
		#if defined _WIN32 || _WIN64
			mHandle = CreateMutex(NULL, FALSE, NULL);
		#else
			 
		#endif
	}

	Mutex::~Mutex()
	{

	}

	void Mutex::Lock()
	{

	}

	void Mutex::Unlock()
	{

	}
}