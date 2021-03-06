#include "AnimationUpdater.h"

#include "Resources/ResourceManifest.h"

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
			const Model& model = mResourceManifest.GetModel(animationInstance.mModelID);
            const Animation& animation = model.GetAnimation(animationInstance.mAnimationID);

			const auto animDuration = animation.GetAnimationDuration();
			animationInstance.mTimestamp += elapsedTime;
			// pause animation if we reach end duration and is not repeating
			if (animationInstance.mTimestamp > animDuration && !animationInstance.mRepeating)
				continue;

			// if repeating start over timestamp
			if (animationInstance.mTimestamp > animDuration && animationInstance.mRepeating)
				animationInstance.mTimestamp = animationInstance.mTimestamp % animDuration;

            const BoneIndex bonesBegin = animationInstance.mBoneRange.first;
            const BoneIndex bonesEnd = animationInstance.mBoneRange.second;

			// root has no parent
			// note: root is always front
            const BoneIndex rootBoneIndex = 0;
			Mat4& rootTransform = mBoneTransforms.at(bonesBegin);
			rootTransform = animation.InterpolateBoneTransform(rootBoneIndex, animationInstance.mTimestamp, animationInstance.mRepeating);

			// interpolate bone animations
            const BoneIndex numBones = bonesEnd - bonesBegin;
			assert(numBones < MAX_NUM_BONES);
            for (BoneIndex boneOffset = 1; boneOffset < numBones; ++boneOffset)
            {
				const BoneIndex bone = boneOffset;
				const BoneIndex parentInstance = bonesBegin + animation.GetParentIndex(bone);
				const Mat4& parentTransform = mBoneTransforms.at(parentInstance);

				const BoneIndex boneInstance = bonesBegin + boneOffset;
				Mat4& transform = mBoneTransforms.at(boneInstance);
				transform = parentTransform * animation.InterpolateBoneTransform(bone, animationInstance.mTimestamp, animationInstance.mRepeating);
            }

			// add misc transforms to get into bone space etc
			// must be done after prior step because of transform multiplication order
			for (BoneIndex boneOffset = 0; boneOffset < numBones; ++boneOffset)
			{
				const BoneIndex bone = boneOffset;
				const Mat4& boneOffsetTransform = animation.GetBoneOffsetTransform(bone);

				const BoneIndex boneInstance = bonesBegin + boneOffset;
				Mat4& transform = mBoneTransforms.at(boneInstance);
				transform = transform * boneOffsetTransform;
			}
        }
    }


    AnimationInstanceID AnimationUpdater::PlayAnimation(const ModelID modelID, const AnimationID animationID, const bool repeating)
    {
        assert(animationID != INVALID_ANIMATION_ID);

		const Model& model = mResourceManifest.GetModel(modelID);
		const Animation& animation = model.GetAnimation(animationID);
		const uint32_t numBonesForAnimation = animation.GetNumberOfBones();
		
		const BoneIndex firstIndex = static_cast<BoneIndex>(mBoneTransforms.size());

		// initialize instance transforms to identity transform
		mBoneTransforms.insert(mBoneTransforms.end(), numBonesForAnimation, gIdentityMatrix);

		const BoneIndex lastIndex = firstIndex + numBonesForAnimation;
		mActiveAnimations.emplace_back(modelID, animationID, firstIndex, lastIndex, repeating);

		// map instance ID to instance index
		const AnimationInstanceID animInstanceID = mAnimInstanceIDGen.GenerateID();
		const AnimationInstanceIndex animInstanceIndex = mActiveAnimations.size() - 1;
		mAnimationInstanceMap.emplace(animInstanceID, animInstanceIndex);

		return animInstanceID;
    }

	void AnimationUpdater::RestartAnimation(const AnimationInstanceID instanceID)
	{
		assert(instanceID != INVALID_ANIMATION_INSTANCE_ID);

		const auto animInstanceIndex = mAnimationInstanceMap.at(instanceID);
		auto& animationInstance = mActiveAnimations.at(animInstanceIndex);

		animationInstance.mTimestamp = Milliseconds(0);
	}

    void AnimationUpdater::StopAnimation(AnimationInstanceID& instanceID)
    {
        assert(instanceID != INVALID_ANIMATION_INSTANCE_ID);

		// copy bone range before deleting instance
		const auto animInstanceIndex = mAnimationInstanceMap.at(instanceID);
		const BoneIndexRange boneRange = mActiveAnimations.at(animInstanceIndex).mBoneRange;

		// remove transforms, update indices, etc
		mActiveAnimations.erase(mActiveAnimations.begin() + animInstanceIndex);
		mAnimationInstanceMap.erase(instanceID);
		mAnimInstanceIDGen.FreeID(instanceID);
		mBoneTransforms.erase(mBoneTransforms.begin() + boneRange.first, mBoneTransforms.begin() + boneRange.second);

		const BoneIndex numBonesRemoved = boneRange.second - boneRange.first;
		for (AnimationInstance& animInstance : mActiveAnimations)
		{
			if (animInstance.mBoneRange.first > boneRange.first)
			{
				animInstance.mBoneRange.first -= numBonesRemoved;
				animInstance.mBoneRange.second -= numBonesRemoved;
			}
		}

		// decrement index-mapping of other animationInstances as we are removing one member
		for (auto& animInstIDIndexPair : mAnimationInstanceMap)
		{
			AnimationInstanceIndex& index = animInstIDIndexPair.second;

			if (index > animInstanceIndex)
				--animInstIDIndexPair.second;
		}
    }


	// NOTE: this adds some wasted transforms - nodes that are animated, but not actually bones, are still present here
	// but it should be very few, if problematic for performance it should be fixed
	const BoneTransforms& AnimationUpdater::GetBonedata() const
	{
		return mBoneTransforms;
	}

	const BoneIndexRange& AnimationUpdater::GetBoneRange(const AnimationInstanceID animationInstance) const
	{
		assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);
		const auto animInstanceIndex = mAnimationInstanceMap.at(animationInstance);

		return mActiveAnimations.at(animInstanceIndex).mBoneRange;
	}
}