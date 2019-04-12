#include "OutputStream.h"

namespace JonsEngine
{
    OutputStream::OutputStream(StreamBuf* buf)
            : std::ostream(buf)
    {
        
    }

    OutputStream::~OutputStream()
    {
        
    }
}
