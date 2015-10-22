#include "include/Resources/ModelAnimation.h"

namespace JonsEngine
{
    ModelAnimation::ModelAnimation(const PackageAnimation& pkgAnimation) : mName(pkgAnimation.mName), mAnimationDuration(Milliseconds(pkgAnimation.mDurationInMilliseconds))
    {
        auto nodeBeginIndex = mNodeTransforms.size();
        for (const PackageAnimatedNode& animNode : pkgAnimation.mAnimatedNodes)
        {
            for (const PackageAnimatedNodeTransform& nodeTransform : animNode.mAnimationTransforms)
                mNodeTransforms.emplace_back(nodeTransform.mTransform, nodeTransform.mTimestampMilliseconds);

            mNodeIDMapping.emplace_back(animNode.mNodeIndex, nodeBeginIndex);
            nodeBeginIndex = mNodeTransforms.size();
        }
    }

    ModelAnimation::~ModelAnimation()
    {
    }


    const Mat4& ModelAnimation::GetNodeTransform(const ModelNodeIndex nodeIndex, const Milliseconds elapsedTime) const
    {
        auto nodeMapIter = std::find_if(mNodeIDMapping.cbegin(), mNodeIDMapping.cend(), [nodeIndex](const NodeIDMap& nodeIDMap) { return nodeIndex == nodeIDMap.first; });
        assert(nodeMapIter != mNodeIDMapping.end());

        //auto nodeTransformIter = std::find_if(mNodeTransforms.cbegin(), mNodeTransforms.cend(), [elapsedTime](const NodeTransformTimestamp& nodeIDMap) { return nodeID == nodeIDMap.first; });
        //assert(nodeTransformIter != mNodeTransforms.cend());

        // TODO...
        return mNodeTransforms.front().first;
    }
    
    ModelAnimationIndex ModelAnimation::GetAnimationIndex() const
    {
        return mAnimationIndex;
    }
    
    Milliseconds ModelAnimation::GetAnimationDuration() const
    {
        return mAnimationDuration;
    }

    const std::string& ModelAnimation::GetName() const
    {
        return mName;
    }
}