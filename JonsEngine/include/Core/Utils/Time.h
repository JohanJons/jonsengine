#pragma once

#if defined _WIN32 || _WIN64
#include <WinSock2.h>
#endif

#include <time.h>
#include <string>

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    uint64_t GetTimeSinceEpoch();
    int32_t GetSecond();
    int32_t GetMinute();
    int32_t GetHour();
    int32_t GetDay();
    int32_t GetMonth();
    int32_t GetYear();
}