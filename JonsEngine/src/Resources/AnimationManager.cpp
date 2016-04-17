#include "include/Resources/AnimationManager.h"

namespace JonsEngine
{
    AnimationManager::AnimationManager()
    {

    }

    AnimationManager::~AnimationManager()
    {
    }


    void AnimationManager::Update(const Milliseconds elapsedTime)
    {
        for (AnimationInstance& animInstance : mActiveAnimations)
        {
            Animation& anim = mAnimations.GetItem(animInstance.mAnimationID);

            // root has no parent
            // note: root is always front
            Mat4& rootTransform = animInstance.mBoneTransforms.front();
            rootTransform = InterpolateTransform(0, elapsedTime);

            const uint32_t numTransforms = animInstance.mBoneTransforms.size();
            for (uint32_t transformNum = 1; transformNum < numTransforms; ++transformNum)
            {
                Mat4& transform = animInstance.mBoneTransforms.at(transformNum);
                const auto parentIndex = animInstance.mParentMap.at(transformNum);
                const Mat4& parentTransform = animInstance.mBoneTransforms.at(parentIndex);

                transform = InterpolateTransform(transformNum, elapsedTime) * parentTransform;
            }
        }
    }


    Mat4& AnimationManager::InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime)
    {
        return Mat4();
    }
}