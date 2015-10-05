#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <utility>

namespace JonsEngine
{
    typedef PackageAnimation::AnimationID AnimationID;
    static const AnimationID INVALID_ANIMATION_ID = PackageAnimation::INVALID_ANIMATION_ID;

    class ModelAnimation
    {
    public:
        ModelAnimation(const PackageAnimation& pkgAnimation);
        ~ModelAnimation();
        
        const Mat4& GetNodeTransform(const ModelNodeID nodeID, const double timestampInSeconds) const;
        const std::string& GetName() const;
        AnimationID GetAnimationID() const;
        double GetTotalDurationInSeconds() const;


    private:
        typedef std::pair<Mat4, double> NodeTransformTimestamp;
        typedef std::vector<NodeTransformTimestamp> NodeTransformsContainer;
        typedef std::pair<ModelNodeID, NodeTransformsContainer::size_type> NodeIDMap;
    
        std::string mName;
        AnimationID mAnimationID;
        double mDurationInSeconds;
        bool mIsPlaying;

        std::vector<NodeIDMap> mNodeIDMapping;
        NodeTransformsContainer mNodeTransforms;
    };
}