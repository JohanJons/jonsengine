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

            const auto& parentMap = animation.GetParentMapping();
            const uint32_t numTransforms = bonesEnd - bonesBegin;
            for (BoneIndex boneIndex = bonesBegin; boneIndex < bonesEnd; ++transformNum)
            {
                Mat4& transform = animInstance.mBoneTransforms.at(transformNum);
                const auto parentIndex = parentMap.at(transformNum);
                const Mat4& parentTransform = animInstance.mBoneTransforms.at(parentIndex);

                transform = InterpolateTransform(transformNum, elapsedTime) * parentTransform;
            }
        }
    }


    AnimationInstanceID AnimationUpdater::PlayAnimation(const AnimationID animationID)
    {
        assert(animationID != INVALID_ANIMATION_ID);

		const Animation& animation = mResourceManifest.GetAnimation(animationID);
		const uint32_t numBonesForAnimation = animation.GetNumberOfBones();
		
		const BoneIndex begin = mBoneTransforms.size();
		const BoneIndex end = begin + numBonesForAnimation;
		mActiveAnimations.emplace_back(animationID, begin, end);

        const AnimationInstanceID animInstanceID = IDGenerator<AnimationInstance>::GetNextID();
		mInstanceMap.emplace(animInstanceID, mActiveAnimations.back());

		return animInstanceID;
    }

    void AnimationUpdater::StopAnimation(AnimationInstanceID& animationInstance)
    {
        assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

		mInstanceMap.erase(animationInstance);
		animationInstance = INVALID_ANIMATION_INSTANCE_ID;

		// TODO: remove transforms, update indices
    }


	const BoneTransforms& AnimationUpdater::GetBonedata() const
	{
		return mBoneTransforms;
	}

	const BoneIndexRange& AnimationUpdater::GetBoneRange(const AnimationInstanceID animationInstance) const
	{
		assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

		return mInstanceMap.at(animationInstance).mBoneRange;
	}

    /*const AnimationUpdater::BoneTransforms& AnimationUpdater::GetBoneData(const AnimationInstanceID animationInstance) const
    {
        assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

        return mActiveAnimations.GetItem(animationInstance).mBoneTransforms;
    }*/


    Mat4 AnimationUpdater::InterpolateTransform(const BoneIndex index, const Milliseconds elapsedTime)
    {
        // TODO
        return Mat4();
    }
}