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


    void AnimationUpdater::Update(const Milliseconds elapsedTime)
    {
        for (AnimationInstance& animInstance : mActiveAnimations)
        {
            const Animation& anim = mResourceManifest.GetAnimation(animInstance.mAnimationID);

            // root has no parent
            // note: root is always front
            Mat4& rootTransform = animInstance.mBoneTransforms.front();
            rootTransform = InterpolateTransform(0, elapsedTime);

            // TODO: only use real number of bones
            const auto& parentMap = anim.GetParentMapping();
            const uint32_t numTransforms = animInstance.mBoneTransforms.size();
            for (uint32_t transformNum = 1; transformNum < numTransforms; ++transformNum)
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
		const AnimationInstanceID animInstanceID = IDGenerator<AnimationInstance>::GetNextID();
		mInstanceMap.emplace(animInstanceID, begin, end);

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

		return mInstanceMap.at(animationInstance);
	}

    /*const AnimationUpdater::BoneTransforms& AnimationUpdater::GetBoneData(const AnimationInstanceID animationInstance) const
    {
        assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

        return mActiveAnimations.GetItem(animationInstance).mBoneTransforms;
    }*/


    Mat4 AnimationUpdater::InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime)
    {
        // TODO
        return Mat4();
    }
}