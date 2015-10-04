#include "include/Resources/ModelAnimation.h"

namespace JonsEngine
{
    ModelAnimation::ModelAnimation(const PackageAnimation& pkgAnimation) : mName(pkgAnimation.mName), mDurationInSeconds(pkgAnimation.mDurationInSeconds)
    {
        auto nodeBeginIndex = mNodeTransforms.size();
        for (const PackageAnimatedNode& animNode : pkgAnimation.mAnimatedNodes)
        {
            for (const PackageAnimatedNodeTransform& nodeTransform : animNode.mAnimationTransforms)
                mNodeTransforms.emplace_back(nodeTransform.mTransform, nodeTransform.mTimestamp);

            mNodeIDMapping.emplace_back(animNode.mNodeID, nodeBeginIndex);
            nodeBeginIndex = mNodeTransforms.size();
        }
    }

    ModelAnimation::~ModelAnimation()
    {
    }


    const Mat4& ModelAnimation::GetNodeTransform(const ModelNodeID nodeID, const double elapsedTimeInSeconds)
    {
        // TODO...
        return mNodeTransforms.front().first;
    }
    
    AnimationID ModelAnimation::GetAnimationID() const
    {
        return mAnimationID;
    }
    
    double ModelAnimation::GetTotalDurationInSeconds() const
    {
        return mDurationInSeconds;
    }

    const std::string& ModelAnimation::GetName() const
    {
        return mName;
    }
}