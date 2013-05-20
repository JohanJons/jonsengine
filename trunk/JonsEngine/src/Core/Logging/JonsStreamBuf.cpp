#include "include/Core/Logging/JonsStreamBuf.h"

#include <algorithm>
#include <functional>

using namespace std;

//
// TODO: change sync/overflow into something better?
//

namespace JonsEngine
{
    JonsStreamBuf::JonsStreamBuf() : streambuf()
    {
        
    }

    JonsStreamBuf::~JonsStreamBuf()
    {
        mStreamBufs.clear();
        mLine.clear();
    }

    void JonsStreamBuf::AddStream(streambuf* sb)
    {
        if (sb)
            mStreamBufs.push_back(sb);
    }

    void JonsStreamBuf::RemoveStream(streambuf* sb)
    {
        if (sb)
            mStreamBufs.remove(sb);
    }

    bool JonsStreamBuf::IsStreamAdded(streambuf* sb) const
    {
        if (sb)
            return (find(mStreamBufs.begin(), mStreamBufs.end(), sb) != mStreamBufs.end());
        else
            return false;
    }

    int32_t JonsStreamBuf::overflow(int32_t c)
    {
        int32_t r1 = 0, r2 = 0;

        if (c == EOF)
           return !EOF;
        else
        {
            mLine += c;
            return r1 == EOF || r2 == EOF ? EOF : c;
        }
    }

    int32_t JonsStreamBuf::sync()
    {
        int32_t res = 0;

        for_each(mStreamBufs.begin(), mStreamBufs.end(), std::bind1st(std::mem_fun(&JonsStreamBuf::SyncCharacter), this));

        mLine.clear();

        return res == 0 ? 0 : -1;
    }

    void JonsStreamBuf::SyncCharacter(streambuf* buffer)
    {
        buffer->sputn(mLine.c_str(), mLine.length());
        buffer->pubsync();
    }
}
