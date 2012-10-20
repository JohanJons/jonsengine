#pragma once

#include "include/Core/EngineDefs.h"

// TODO: better way than to include windows header
#if defined _WIN32 || _WIN64
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif ANDROID
    #include <pthread.h>
#endif

namespace JonsEngine
{
    class ILogManager;

    /* Mutex definition */
    class Mutex
    {
    public:
        enum MutexState
        {
            UNLOCKED = 0,
            LOCKED
        };

        #if defined _WIN32 || _WIN64
            typedef CRITICAL_SECTION MutexHandle;
        #elif ANDROID
            typedef pthread_mutex_t MutexHandle;
        #endif

        Mutex();
        ~Mutex();

        int32_t Lock();
        int32_t Unlock();

        const MutexState& GetMutexState() const;
        MutexHandle& GetMutexHandle();

    private:
        MutexHandle mHandle;
        MutexState mState;
    };


    /* Mutex inlines */
    inline const Mutex::MutexState& Mutex::GetMutexState() const        { return mState; }
    inline Mutex::MutexHandle& Mutex::GetMutexHandle()                  { return mHandle; }
}