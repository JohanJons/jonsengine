#pragma once

#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{
    class Mutex;

    /* ScopedLock definition */
    class ScopedLock
    {
    public:
        ScopedLock(Mutex& mutex);
        ~ScopedLock();

    private:
        Mutex& mMutex;
    };


    /* ScopedLock inlines */
    inline ScopedLock::ScopedLock(Mutex& mutex) : mMutex(mutex)     { mMutex.Lock();    }
    inline ScopedLock::~ScopedLock()                                { mMutex.Unlock();  }
}