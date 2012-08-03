#ifndef _JONS_MUTEX_H
#define _JONS_MUTEX_H

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
#endif

namespace JonsEngine
{
	class Mutex
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef HANDLE MutexHandle;
		#else
			typedef pthread_mutex_t MutexHandle;
		#endif

		Mutex();
		~Mutex();

		void Lock();
		void Unlock();

	private:
		MutexHandle mHandle;
	};
}

#endif