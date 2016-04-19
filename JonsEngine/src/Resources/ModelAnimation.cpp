#include "include/Resources/ModelAnimation.h"

#include "include/Core/Math/Math.h"
#include "include/Core/Logging/Logger.h"
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
        for (const PackageBoneAnimation& animNode : pkgAnimation.mBoneAnimations)
        {
            for (const PackageAnimationKeyframe& keyframe : animNode.mKeyframes)
                mAnimationTransforms.emplace_back(keyframe.mTransform, Milliseconds(keyframe.mTimestampMilliseconds));

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
        if (elapsedTime >= mAnimationDuration)
            return gIdentityMatrix;

        const NodeAnimTransformRange& transformRange = mNodeAnimTransformMap.at(nodeIndex);
        const AnimTransformIndex startIndex = transformRange.first;
        const AnimTransformIndex endIndex = transformRange.second;

        assert(endIndex >= startIndex);
        assert(endIndex <= mAnimationTransforms.size());

        // zero range means no animation transforms
        if (startIndex == endIndex)
            return gIdentityMatrix;
        
        // TODO: remove searching
        const auto firstNotElapsedTransform = std::find_if(mAnimationTransforms.cbegin() + startIndex, mAnimationTransforms.cbegin() + endIndex,
            [elapsedTime](const AnimTransform& transform) { return elapsedTime <= transform.second; });

        // TODO - interpolate...
        return (*firstNotElapsedTransform).first;
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