#include "include/Resources/ModelAnimation.h"

#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    ModelAnimation::ModelAnimation(const PackageAnimation& pkgAnimation, const uint32_t maxNumNodes) :
        mName(pkgAnimation.mName),
        mAnimationDuration(Milliseconds(pkgAnimation.mDurationInMilliseconds)),
        mNodeAnimTransformMap(maxNumNodes)
    {
        // std::pair default constructor guarantees basic types are initialized
        // so even if an index in mNodeAnimTransformMap isnt touched the range will be zero

        auto animRangeBeginIndex = mAnimationTransforms.size();
        for (const PackageAnimatedNode& animNode : pkgAnimation.mAnimatedNodes)
        {
            for (const PackageAnimatedNodeTransform& nodeTransform : animNode.mAnimationTransforms)
                mAnimationTransforms.emplace_back(nodeTransform.mTransform, nodeTransform.mTimestampMilliseconds);

            const auto animRangeEndIndex = mAnimationTransforms.size();
            mNodeAnimTransformMap.at(animNode.mNodeIndex) = NodeAnimTransformRange(animRangeBeginIndex, animRangeEndIndex);
            animRangeBeginIndex = animRangeEndIndex;
        }
    }

    ModelAnimation::~ModelAnimation()
    {
    }


    const Mat4& ModelAnimation::GetNodeTransform(const ModelNodeIndex nodeIndex, const Milliseconds elapsedTime) const
    {
        // make sure elapsted time isn't out of bounds
        assert(mAnimationDuration >= elapsedTime);

        const NodeAnimTransformRange& transformRange = mNodeAnimTransformMap.at(nodeIndex);
        const AnimTransformIndex startIndex = transformRange.first;
        const AnimTransformIndex endIndex = transformRange.second;

        assert(endIndex >= startIndex);
        assert(endIndex <= mAnimationTransforms.size());

        // zero range means no animation transforms
        if (startIndex == endIndex)
            return gIdentityMatrix;

        const auto firstNotElapsedTransform = std::find_if(mAnimationTransforms.cbegin() + startIndex, mAnimationTransforms.cbegin() + endIndex,
            [elapsedTime]() {});


        //auto nodeMapIter = std::find_if(mNodeIDMapping.cbegin(), mNodeIDMapping.cend(), [nodeIndex](const NodeIDMap& nodeIDMap) { return nodeIndex == nodeIDMap.first; });
        //assert(nodeMapIter != mNodeIDMapping.end());

       // auto nodeTransformIter = std::find_if(mNodeTransforms.cbegin(), mNodeTransforms.cend(), [elapsedTime](const NodeTransformTimestamp& nodeIDMap) { return nodeID == nodeIDMap.first; });
        //assert(nodeTransformIter != mNodeTransforms.cend());

        // TODO...
        return mNodeTransforms.front().first;
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