#include "Resources/Animation.h"

#include "Resources/JonsPackage.h"

namespace JonsEngine
{
    Animation::Animation(const PackageAnimation& pkgAnimation, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
        mName(pkgAnimation.mName),
        mAnimationDuration(pkgAnimation.mDurationInMilliseconds),
        mInverseRootMatrix(pkgAnimation.mInverseRootMatrix),
        mParentMap(parentMap),
        mBoneOffsetTransforms(boneOffsets)
    {
		for (const auto& pkgBoneAnims : pkgAnimation.mBoneAnimations)
		{
			mKeyframes.emplace_back();
			auto& keyframeContainer = mKeyframes.back();

			for (const auto& pkgKeyframe : pkgBoneAnims)
				keyframeContainer.emplace_back(pkgKeyframe);
		}
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

    Mat4 Animation::InterpolateBoneTransform(const BoneIndex bone, const Milliseconds currTime, const bool repeatingAnimation) const
    {
        assert(bone != INVALID_BONE_INDEX);

		auto& keyframeContainer = mKeyframes.at(bone);
		if (mKeyframes.empty())
			return gIdentityMatrix;

		const KeyframeIterator endIter = keyframeContainer.end();
		const KeyframeIterator currframeIter = GetBoneKeyframe(keyframeContainer, currTime);
		assert(currframeIter != endIter);

		const bool isEndFrameAndNoRepeat = (currframeIter == endIter || currframeIter + 1 == endIter) && !repeatingAnimation;
		if (keyframeContainer.size() == 1 || isEndFrameAndNoRepeat)
		{
            // only last keyframe is used - no interpolation
            const BoneKeyframe& lastKeyframe = keyframeContainer.back();
            
			Mat4 translationMatrix = glm::translate(gIdentityMatrix, lastKeyframe.mTranslation);
			Mat4 rotationMatrix = glm::toMat4(lastKeyframe.mRotation);
            
            return translationMatrix * rotationMatrix;;
		}

        const KeyframeIterator nextFrameIter = GetNextFrameIter(keyframeContainer, currframeIter);
        const float interpolationFactor = GetInterpolationFactor(keyframeContainer, currframeIter, nextFrameIter, currTime);
		assert(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f);

        // interpolate rotation
        const Quaternion& currRot = currframeIter->mRotation;
        const Quaternion& nextRot = nextFrameIter->mRotation;
        Quaternion finalRot = glm::slerp(nextRot, currRot, interpolationFactor);
		finalRot = glm::normalize(finalRot);
        
        // interpolate translation
        const Vec3& currTranslation = currframeIter->mTranslation;
        const Vec3& nextTranslation = nextFrameIter->mTranslation;
		const Vec3 finalTranslation = glm::mix(currTranslation, nextTranslation, interpolationFactor);

		Mat4 translationMatrix = glm::translate(gIdentityMatrix, finalTranslation);
		Mat4 rotationMatrix = glm::toMat4(finalRot);
        
        return translationMatrix * rotationMatrix;
    }


	Animation::KeyframeIterator Animation::GetBoneKeyframe(const KeyframeContainer& keyframeContainer, const Milliseconds time) const
    {
		assert(!keyframeContainer.empty());

		const auto endIter = keyframeContainer.end();
		if (keyframeContainer.size() == 1)
			return keyframeContainer.begin();

		for (auto iter = keyframeContainer.begin() + 1; iter != endIter; ++iter)
		{
			if (time <= iter->mTimestamp)
				return iter - 1;
		}

        return endIter;
    }

	Animation::KeyframeIterator Animation::GetNextFrameIter(const KeyframeContainer& keyframeContainer, const KeyframeIterator currFrameIter) const
	{
		const auto endIter = keyframeContainer.end();
        assert(currFrameIter != endIter);
        
        // if we are at the end, interpolate at the beginning again
		if (currFrameIter + 1 == endIter)
			return keyframeContainer.begin();

		return currFrameIter + 1;
	}
    
    float Animation::GetInterpolationFactor(const KeyframeContainer& keyframeContainer, const KeyframeIterator currIter, const KeyframeIterator nextIter, const Milliseconds currElapsedTime) const
    {
        const auto endIter = keyframeContainer.end();
        assert(currIter != endIter && nextIter != endIter);
        
        const auto currTimestamp = currIter->mTimestamp;
        // if we need to loop animation, fake timestamp of nextIter to make interpolation calculations make sense
        const auto nextTimestamp = currIter + 1 == endIter? currTimestamp + nextIter->mTimestamp : nextIter->mTimestamp;
		assert(currTimestamp != nextTimestamp);

        const float deltaTime = (nextTimestamp - currTimestamp).count();
        const float interpolationFactor = (nextTimestamp - currElapsedTime).count() / deltaTime;
        
        return interpolationFactor;
    }
}