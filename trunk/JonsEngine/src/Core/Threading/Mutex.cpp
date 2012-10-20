#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{

    Mutex::Mutex() : mState(UNLOCKED)
    {
        #if defined _WIN32 || _WIN64
            InitializeCriticalSection(&mHandle);
        #elif ANDROID
            pthread_mutex_init(&mHandle, NULL);
        #endif
    }

    Mutex::~Mutex()
    {
        #if defined _WIN32 || _WIN64
            DeleteCriticalSection(&mHandle);
        #elif ANDROID
            pthread_mutex_destroy(&mHandle);
        #endif
    }

    int32_t Mutex::Lock()
    {
        int32_t ret = 0;

        #if defined _WIN32 || _WIN64
            EnterCriticalSection(&mHandle);
        #elif ANDROID
            ret = pthread_mutex_lock(&mHandle);
        #endif

        if (!ret)
            mState = LOCKED;

        return ret;
    }

    int32_t Mutex::Unlock()
    {
        int32_t ret = 0;

        #if defined _WIN32 || _WIN64
            LeaveCriticalSection(&mHandle);
        #elif ANDROID
            ret = pthread_mutex_unlock(&mHandle);
        #endif

        if (!ret)
            mState = UNLOCKED;

        return ret;
    }
}