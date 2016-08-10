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

    Mat4 Animation::InterpolateBoneTransform(const BoneIndex bone, const Milliseconds elapsedTime) const
    {
        /*
         // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }

        uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        uint NextRotationIndex = (RotationIndex + 1);
        assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
        
        uint Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            assert(pNodeAnim->mNumRotationKeys > 0);

            for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
                if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
                    return i;
                }
            }

            assert(0);
        }
        */
		if (mKeyframes.empty())
			return gIdentityMatrix;

		const KeyframeIterator currframeIter = GetBoneKeyframe(elapsedTime);
		/*const KeyframeIterator nextFrameIter = GetNextFrameIter(currframeIter);
        const auto endIter = mKeyframes.end();
		if (currframeIter == endIter || nextFrameIter == endIter)
		{
            // only last keyframe is used - no interpolation
            const BoneKeyframe& lastKeyframe = mKeyframes.back();
            Mat4 boneMatrix = glm::toMat4(lastKeyframe.mRotation);
            boneMatrix = glm::translate(boneMatrix, lastKeyframe.mTranslation);
            
            return boneMatrix;
        }
        else
        {
            const float deltaTime = (nextFrameIter->mTimestamp - currframeIter->mTimestamp).count();
            const float interpolationFactor = (elapsedTime - currframeIter->mTimestamp).count() / deltaTime;
        
            // interpolate rotation
            const Quaternion& currRot = currframeIter->mRotation;
            const Quaternion& nextRot = nextFrameIter->mRotation;
            const Quaternion finalRot = glm::slerp(currRot, nextRot, interpolationFactor);
            
            // interpolate translation
            const Vec3& currTranslation = currframeIter->mTranslation;
            const Vec3& nextTranslation = nextFrameIter->mTranslation;
            const Vec3 finalTranslation = interpolationFactor * currTranslation + (1.0f - interpolationFactor) * nextTranslation;
        }*/

        // TODO: interpolation between frames
        Mat4 boneMatrix = glm::toMat4(currframeIter->mRotation);
        boneMatrix = glm::translate(boneMatrix, currframeIter->mTranslation);

        //return boneMatrix;
		return gIdentityMatrix;
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

	Animation::KeyframeIterator Animation::GetNextFrameIter(const KeyframeIterator& currFrameIter) const
	{
		const auto endIter = mKeyframes.end();
		if (currFrameIter == endIter)
			return endIter;

		return endIter;
	}
}