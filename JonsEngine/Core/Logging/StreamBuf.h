#pragma once

#include <iostream>
#include <algorithm>
#include <list>

#include "Types.h"

namespace JonsEngine
{
    class StreamBuf : public std::streambuf
    {
    public:
        StreamBuf();
        ~StreamBuf();

        void AddStream(std::streambuf* sb);
        void RemoveStream(std::streambuf* sb);
        bool IsStreamAdded(std::streambuf* sb) const;

    private:
        void SyncCharacter(std::streambuf*);

        std::list<std::streambuf*> mStreamBufs;
        std::string mLine;

        int32_t overflow(int32_t c);
        int32_t sync();

    };
}
