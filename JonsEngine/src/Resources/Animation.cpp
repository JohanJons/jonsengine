#include "include/Resources/Animation.h"

#include "include/Resources/JonsPackage.h"

namespace JonsEngine
{
    Animation::Animation(const PackageAnimation& pkgAnimation, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
        mName(pkgAnimation.mName),
        mAnimationDuration(pkgAnimation.mDurationInMilliseconds),
        mInverseRootMatrix(pkgAnimation.mInverseRootMatrix),
        mParentMap(parentMap),
        mBoneOffsetTransforms(boneOffsets)
    {
        for (const auto& keyframe : pkgAnimation.mKeyframes)
            mKeyframes.emplace_back(keyframe);
    }

    Animation::Animation(const Animation& other, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
        mName(other.mName),
        mAnimationDuration(other.mAnimationDuration),
        mInverseRootMatrix(other.mInverseRootMatrix),
        mParentMap(parentMap),
        mBoneOffsetTransforms(boneOffsets),
        mKeyframes(other.mKeyframes)
    {
    }
    

    const std::string& Animation::GetName() const
    {
        return mName;
    }

    Milliseconds Animation::GetAnimationDuration() const
    {
        return mAnimationDuration;
    }

    uint32_t Animation::GetNumberOfBones() const
    {
        return mBoneOffsetTransforms.size();
    }


    const Mat4& Animation::GetInverseRootMatrix() const
    {
        return mInverseRootMatrix;
    }

    const Mat4& Animation::GetBoneOffsetTransform(const BoneIndex bone) const
    {
        return mBoneOffsetTransforms.at(bone);
    }

    const BoneIndex Animation::GetParentIndex(const BoneIndex bone) const
    {
        return mParentMap.at(bone);
    }

    Mat4 Animation::InterpolateBoneTransform(const BoneIndex bone, const Milliseconds elapsedTime, const bool repeatingAnimation) const
    {
        assert(bone != INVALID_BONE_INDEX);
		if (mKeyframes.empty())
			return gIdentityMatrix;

		const KeyframeIterator currframeIter = GetBoneKeyframe(elapsedTime);
		const KeyframeIterator endIter = mKeyframes.end();
		const bool isEndFrameAndNoRepeat = (currframeIter == endIter || currframeIter + 1 == endIter) && !repeatingAnimation;
		if (mKeyframes.size() == 1 || isEndFrameAndNoRepeat)
		{
            // only last keyframe is used - no interpolation
            const BoneKeyframe& lastKeyframe = mKeyframes.back();
            
            Mat4 boneMatrix = glm::toMat4(lastKeyframe.mRotation);
            boneMatrix = glm::translate(boneMatrix, lastKeyframe.mTranslation);
            
            return boneMatrix;
		}

        const KeyframeIterator nextFrameIter = GetNextFrameIter(currframeIter);
        const float interpolationFactor = GetInterpolationFactor(currframeIter, nextFrameIter, elapsedTime);
        assert(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f);
        
        // interpolate rotation
        const Quaternion& currRot = currframeIter->mRotation;
        const Quaternion& nextRot = nextFrameIter->mRotation;
        Quaternion finalRot = glm::slerp(currRot, nextRot, interpolationFactor);
        finalRot = glm::normalize(finalRot);
        
        // interpolate translation
        const Vec3& currTranslation = currframeIter->mTranslation;
        const Vec3& nextTranslation = nextFrameIter->mTranslation;
        const Vec3 finalTranslation = interpolationFactor * currTranslation + (1.0f - interpolationFactor) * nextTranslation;
        
        Mat4 boneMatrix = glm::toMat4(finalRot);
        boneMatrix = glm::translate(boneMatrix, finalTranslation);
        
        return boneMatrix;
    }


	Animation::KeyframeIterator Animation::GetBoneKeyframe(const Milliseconds time) const
    {
		const auto endIter = mKeyframes.end();
		for (auto iter = mKeyframes.begin(); iter != endIter; ++iter)
		{
			if (iter->mTimestamp > time)
				return iter;
		}

        return endIter;
    }

	Animation::KeyframeIterator Animation::GetNextFrameIter(const KeyframeIterator currFrameIter) const
	{
		const auto endIter = mKeyframes.end();
        assert(currFrameIter != endIter);
        
        // if we are at the end, interpolate at the beginning again
		if (currFrameIter + 1 == endIter)
			return mKeyframes.begin();

		return currFrameIter + 1;
	}
    
    float Animation::GetInterpolationFactor(const KeyframeIterator currIter, const KeyframeIterator nextIter, const Milliseconds elapsedTime) const
    {
        const auto endIter = mKeyframes.end();
        assert(currIter != endIter && nextIter != endIter);
        
        const auto currTimestamp = currIter->mTimestamp;
        // if we need to loop animation, fake timestamp of nextIter to make interpolation calculations make sense
        const auto nextTimestamp = currIter + 1 == endIter? currTimestamp + nextIter->mTimestamp : nextIter->mTimestamp;
    
        const float deltaTime = (nextTimestamp - currTimestamp).count();
        const float interpolationFactor = (elapsedTime - currTimestamp).count() / deltaTime;
        
        return interpolationFactor;
    }
}