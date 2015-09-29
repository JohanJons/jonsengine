#pragma once

#include "include/Core/Types.h"

#include <string>

namespace JonsEngine
{
    class Animation
    {
    public:
        Animation();
        ~Animation();

        void Play(const bool doPlay);

        bool IsPlaying() const;
        double GetTotalDurationInSeconds() const;
        const std::string& GetName() const;


    private:
        std::string mName;
        double mDurationInSeconds;
        bool mIsPlaying;
    };
}