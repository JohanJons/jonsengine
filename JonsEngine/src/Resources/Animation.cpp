#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix, const BoneAnimationContainer& boneAnimations, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
        mName(name),
        mAnimationDuration(duration),
        mInverseRootMatrix(inverseRootMatrix),
		mParentMap(parentMap),
		mBoneOffsetTransforms(boneOffsets),
		mBoneAnimations(boneAnimations)
    {
    }

	Animation::Animation(const Animation& other, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
		mName(other.mName),
		mAnimationDuration(other.mAnimationDuration),
		mInverseRootMatrix(other.mInverseRootMatrix),
		mParentMap(parentMap),
		mBoneOffsetTransforms(boneOffsets),
		mBoneAnimations(other.mBoneAnimations)
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
		return mBoneAnimations.size();
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

		const BoneAnimation& boneAnimation = mBoneAnimations.at(bone);
		const BoneKeyframe& keyframe = boneAnimation.GetBoneKeyframe(elapsedTime);

		// TODO: interpolation between frames
		Mat4 boneMatrix = glm::toMat4(keyframe.mRotation);

		return gIdentityMatrix;
	}
}