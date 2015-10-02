#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Resources/ModelAnimation.h"

#include <string>

namespace JonsEngine
{
    class AnimationInstance
    {
    public:
        AnimationInstance();
        ~AnimationInstance();

        void Play(const bool doPlay);

        bool IsPlaying() const;
        double GetTotalDurationInSeconds() const;
        const std::string& GetName() const;


    private:
        //ModelAnimation
        bool mIsPlaying;
    };

    typedef IDMap<AnimationInstance>::ItemID AnimationInstanceID;
    static const AnimationInstanceID INVALID_ANIMATION_INSTANCE_ID = IDMap<AnimationInstance>::INVALID_ITEM_ID;
}