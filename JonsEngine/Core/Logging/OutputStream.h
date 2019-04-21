#pragma once

#include <iostream>

#include "Core/Logging/StreamBuf.h"

namespace JonsEngine
{
    class OutputStream : public std::ostream
    {
    public:
        OutputStream(StreamBuf* buf);
        ~OutputStream();

    };
}
