#pragma once

#include <iostream>
#include <algorithm>
#include <list>

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    class JonsStreamBuf : public std::streambuf
    {
    public:
        JonsStreamBuf();
        ~JonsStreamBuf();

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
