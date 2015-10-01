#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <utility>

namespace JonsEngine
{
    class ModelAnimation
    {
    public:
        typedef std::pair<double, Mat4> TimestampTransform;
        typedef std::pair<ModelNode::NodeID, TimestampTransform> AnimatedNodeTransform;
    
        ModelAnimation(const PackageAnimation& pkgAnimation);
        ~ModelAnimation();

        bool IsPlaying() const;
        double GetTotalDurationInSeconds() const;
        const std::string& GetName() const;


    private:
        std::string mName;
        double mDurationInSeconds;
        bool mIsPlaying;

        std::vector<AnimatedNodeTransform> mAnimatedNodes;
    };
}