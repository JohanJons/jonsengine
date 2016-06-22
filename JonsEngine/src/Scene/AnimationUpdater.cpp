#include "include/Scene/AnimationUpdater.h"

#include "include/Resources/ResourceManifest.h"

namespace JonsEngine
{
    AnimationUpdater::AnimationUpdater(const ResourceManifest& resourceManifest) :
        mResourceManifest(resourceManifest)
    {
    }

    AnimationUpdater::~AnimationUpdater()
    {
    }


    // TODO: animation instances should be sorted by animation id for performance
    void AnimationUpdater::Update(const Milliseconds elapsedTime)
    {
        for (AnimationInstance& animationInstance : mActiveAnimations)
        {
            const Animation& animation = mResourceManifest.GetAnimation(animationInstance.mAnimationID);

            const BoneIndex bonesBegin = animationInstance.mBoneRange.first;
            const BoneIndex bonesEnd = animationInstance.mBoneRange.second;

            // root has no parent
            // note: root is always front
            Mat4& rootTransform = mBoneTransforms.at(bonesBegin);
            rootTransform = InterpolateTransform(bonesBegin, elapsedTime);

            const uint32_t numTransforms = bonesEnd - bonesBegin;
            for (uint32_t boneOffset = 0; boneOffset < numTransforms; ++boneOffset)
            {
                Mat4& transform = mBoneTransforms.at(bonesBegin + boneOffset);
				
                const BoneIndex parentIndex = animation.GetParentIndex(boneOffset);
                const Mat4& parentTransform = mBoneTransforms.at(parentIndex);

                transform = InterpolateTransform(boneOffset, elapsedTime) * parentTransform;
            }
        }
    }


    AnimationInstanceID AnimationUpdater::PlayAnimation(const AnimationID animationID)
    {
        assert(animationID != INVALID_ANIMATION_ID);

		const Animation& animation = mResourceManifest.GetAnimation(animationID);
		const uint32_t numBonesForAnimation = animation.GetNumberOfBones();
		
		const BoneIndex firstIndex = mBoneTransforms.size();

		// initialize instance transforms to bind pose transforms
		const auto& bindPoseTransforms = animation.GetBindPoseTransforms();
		mBoneTransforms.insert(mBoneTransforms.end(), bindPoseTransforms.begin(), bindPoseTransforms.end());

		const BoneIndex lastIndex = firstIndex + numBonesForAnimation;
		mActiveAnimations.emplace_back(animationID, firstIndex, lastIndex);

		// map instance ID
		const AnimationInstanceID animInstanceID = mAnimInstanceIDGen.GenerateID();
		mAnimationInstanceMap.emplace(animInstanceID, mActiveAnimations.back());

		return animInstanceID;
    }

    void AnimationUpdater::StopAnimation(AnimationInstanceID& instanceID)
    {
        assert(instanceID != INVALID_ANIMATION_INSTANCE_ID);

		// copy bone range before deleting instance
		const BoneIndexRange boneRange = mAnimationInstanceMap.at(instanceID).mBoneRange;

		mAnimationInstanceMap.erase(instanceID);
		mAnimInstanceIDGen.FreeID(instanceID);

		// remove transforms, update indices, etc
		mBoneTransforms.erase(mBoneTransforms.begin() + boneRange.first, mBoneTransforms.begin() + boneRange.second);

		const std::size_t numBonesRemoved = boneRange.second - boneRange.first;
		for (AnimationInstance& animInstance : mActiveAnimations)
		{
			if (animInstance.mBoneRange.first > boneRange.first)
			{
				animInstance.mBoneRange.first -= numBonesRemoved;
				animInstance.mBoneRange.second -= numBonesRemoved;
			}
		}
    }


	const BoneTransforms& AnimationUpdater::GetBonedata() const
	{
		return mBoneTransforms;
	}

	const BoneIndexRange& AnimationUpdater::GetBoneRange(const AnimationInstanceID animationInstance) const
	{
		assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

		return mAnimationInstanceMap.at(animationInstance).mBoneRange;
	}


    Mat4 AnimationUpdater::InterpolateTransform(const BoneIndex index, const Milliseconds elapsedTime)
    {
        // TODO
        return Mat4(1.0f);
    }
}