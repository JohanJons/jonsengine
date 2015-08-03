#pragma once

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

        const std::string mName;
        const double mDurationInSeconds;


    private:
        bool mIsPlaying;
    };
}