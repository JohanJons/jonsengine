#include "include/Core/Logging/StreamBuf.h"

#include <algorithm>
#include <functional>

using namespace std;

//
// TODO: change sync/overflow into something better?
//

namespace JonsEngine
{
    StreamBuf::StreamBuf() : streambuf()
    {
    }

    StreamBuf::~StreamBuf()
    {
        mStreamBufs.clear();
        mLine.clear();
    }

    void StreamBuf::AddStream(streambuf* sb)
    {
        if (sb)
            mStreamBufs.push_back(sb);
    }

    void StreamBuf::RemoveStream(streambuf* sb)
    {
        if (sb)
            mStreamBufs.remove(sb);
    }

    bool StreamBuf::IsStreamAdded(streambuf* sb) const
    {
        if (sb)
            return (find(mStreamBufs.begin(), mStreamBufs.end(), sb) != mStreamBufs.end());
        else
            return false;
    }

    int32_t StreamBuf::overflow(int32_t c)
    {
        int32_t r1 = 0, r2 = 0;

        if (c == EOF)
           return !EOF;
        else
        {
            mLine += c;
            return c;
        }
    }

    int32_t StreamBuf::sync()
    {
        int32_t res = 0;

        for_each(mStreamBufs.begin(), mStreamBufs.end(), std::bind1st(std::mem_fun(&StreamBuf::SyncCharacter), this));

        mLine.clear();

        return res == 0 ? 0 : -1;
    }

    void StreamBuf::SyncCharacter(streambuf* buffer)
    {
        buffer->sputn(mLine.c_str(), mLine.length());
        buffer->pubsync();
    }
}
