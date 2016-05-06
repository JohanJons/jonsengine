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
        return mActiveAnimations.Insert(animationID);
    }

    void AnimationHandler::StopAnimation(AnimationInstanceID& animationInstance)
    {
        mActiveAnimations.Erase(animationInstance);
    }
}