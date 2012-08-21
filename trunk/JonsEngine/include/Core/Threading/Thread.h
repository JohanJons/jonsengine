#ifndef _JONS_THREAD_H
#define _JONS_THREAD_H

#include "interface/Core/Threading/IThread.h"

namespace JonsEngine
{
	/* Thread wrapper class */
	class Thread : public IThread
	{
	public:
		struct ThreadInfo
		{
			ThreadState mState;
			void* (*mFunctionPointer) (void*);
			void* mArg;
		};

		Thread(IMemoryAllocator& allocator, ILogManager& logger);
		Thread(void* (*start) (void*), void* arg, IMemoryAllocator& allocator, ILogManager& logger);
		~Thread();

		Thread& operator=(Thread& other);
		int32_t Join();
		int32_t SetPriority(int32_t priority);

		ThreadHandle& GetNativeHandle();
		ThreadState GetThreadState() const;

	private:
		ThreadHandle mHandle;
		ThreadInfo* mThreadInfo;
		IMemoryAllocator& mAllocator;
		ILogManager& mLogger;

		int32_t Destroy();
		int32_t Detach();
		static void* Run(void* arg);
		ThreadHandle _CreateThread(void* (*start) (void*), void* arg);
		void _JoinThread(ThreadHandle& handle);

	};
}

#endif