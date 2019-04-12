#pragma once

#include <iostream>

#include "StreamBuf.h"

namespace JonsEngine
{
    class OutputStream : public std::ostream
    {
    public:
        OutputStream(StreamBuf* buf);
        ~OutputStream();

    };
}
