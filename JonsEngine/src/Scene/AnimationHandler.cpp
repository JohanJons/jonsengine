#include "include/Scene/AnimationHandler.h"

#include "include/Resources/ResourceManifest.h"

namespace JonsEngine
{
    AnimationHandler::AnimationHandler(const ResourceManifest& resourceManifest) :
        mResourceManifest(resourceManifest)
    {
    }

    AnimationHandler::~AnimationHandler()
    {
    }


    void AnimationHandler::Update(const Milliseconds elapsedTime)
    {
        for (AnimationInstance& animInstance : mActiveAnimations)
        {
            const Animation& anim = mResourceManifest.GetAnimation(animInstance.mAnimationID);

            // root has no parent
            // note: root is always front
            Mat4& rootTransform = animInstance.mBoneTransforms.front();
            rootTransform = InterpolateTransform(0, elapsedTime);

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


    AnimationInstanceID AnimationHandler::PlayAnimation(const AnimationID animationID)
    {
        assert(animationID != INVALID_ANIMATION_ID);

        return mActiveAnimations.Insert(animationID);
    }

    void AnimationHandler::StopAnimation(AnimationInstanceID& animationInstance)
    {
        assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

        mActiveAnimations.Erase(animationInstance);
    }

    const AnimationInstance::BoneData& AnimationHandler::GetBoneData(const AnimationInstanceID animationInstance) const
    {
        assert(animationInstance != INVALID_ANIMATION_INSTANCE_ID);

        mActiveAnimations.GetItem(animationInstance).mBoneTransforms;
    }


    Mat4 AnimationHandler::InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime)
    {
        return Mat4();
    }
}