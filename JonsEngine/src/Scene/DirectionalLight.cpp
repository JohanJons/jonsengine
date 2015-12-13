#include "include/Scene/DirectionalLight.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    DirectionalLight::DirectionalLight(const std::string& name, const uint32_t numShadowmapCascades) :
        mName(name), mNumShadowmapCascades(numShadowmapCascades), mLightColor(1.0f), mLightDirection(0.0f, -1.0f, -1.0f), mCascadeSplitLambda(0.8f)
    {
        assert(numShadowmapCascades >= 1 && numShadowmapCascades <= 4);

        mCascadeKDOPRange.reserve(numShadowmapCascades);
        mSplitDistances.reserve(numShadowmapCascades);
    }


    void DirectionalLight::SetLightDirection(const Vec3& direction)
    {
        mLightDirection = direction;
    }

    void DirectionalLight::SetLightColor(const Vec4& lightColor)
    {
        mLightColor = lightColor;
    }


    void DirectionalLight::SetCascadeSplitLambda(const float lambda)
    {
        assert(lambda <= 1.0f && lambda >= 0.0f);

        mCascadeSplitLambda = lambda;
    }

    void DirectionalLight::UpdateCascadesBoundingVolume(const Mat4& viewMatrix, const float degreesFOV, const float aspectRatio, const float minDepth, const float maxDepth)
    {
        mKDOP.clear();
        mCascadeKDOPRange.clear();

        UpdateSplitDistances(minDepth, maxDepth);

        for (uint32_t cascadeIndex = 0; cascadeIndex < mNumShadowmapCascades; ++cascadeIndex)
        {
            const float nearZ = mSplitDistances[cascadeIndex];
            const float farZ = mSplitDistances[cascadeIndex + 1];

            const Mat4 cascadePerspectiveMatrix = PerspectiveMatrixFov(degreesFOV, aspectRatio, nearZ, farZ);
            const Mat4 cascadeViewProjMatrix = cascadePerspectiveMatrix * viewMatrix;

            const FrustumPlanes frustumPlanes = GetFrustumPlanes(cascadeViewProjMatrix);
            const FrustumCorners frustumCorners = GetFrustumCorners(cascadeViewProjMatrix);

            UpdateKDOP(frustumPlanes, frustumCorners);
        }
    }


    DirectionalLight::BoundingVolume DirectionalLight::GetBoundingVolume(const uint32_t cascadeIndex) const
    {
        assert(cascadeIndex < mNumShadowmapCascades);

        const size_t startIndex = cascadeIndex == 0 ? 0 : mCascadeKDOPRange.at(cascadeIndex - 1);
        const size_t endIndex = mCascadeKDOPRange.at(cascadeIndex);

        return BoundingVolume(mKDOP, startIndex, endIndex);
    }

    void DirectionalLight::GetSplitDistance(const uint32_t cascadeIndex, float& nearZ, float& farZ) const
    {
        assert(cascadeIndex < mNumShadowmapCascades);

        nearZ = mSplitDistances[cascadeIndex];
        farZ = mSplitDistances[cascadeIndex + 1];
    }


    const std::string& DirectionalLight::GetName() const
    {
        return mName;
    }

    uint32_t DirectionalLight::GetNumCascades() const
    {
        return mNumShadowmapCascades;
    }

    const Vec4& DirectionalLight::GetLightColor() const
    {
        return mLightColor;
    }

    const Vec3& DirectionalLight::GetLightDirection() const
    {
        return mLightDirection;
    }


    void DirectionalLight::UpdateSplitDistances(const float minDepth, const float maxDepth)
    {
        mSplitDistances.clear();

        const float nearClip = Z_NEAR;
        const float farClip = Z_FAR;
        const float clipRange = farClip - nearClip;

        const float minZ = nearClip + minDepth * clipRange;
        const float maxZ = nearClip + maxDepth * clipRange;

        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

        mSplitDistances.push_back(minZ);
        for (uint32_t cascadeIndex = 0; cascadeIndex < mNumShadowmapCascades; ++cascadeIndex)
        {
            const float p = (cascadeIndex + 1) / static_cast<float>(mNumShadowmapCascades);
            const float log = minZ * std::pow(ratio, p);
            const float uniform = minZ + range * p;
            const float d = mCascadeSplitLambda * (log - uniform) + uniform;

            mSplitDistances.push_back(((d - nearClip) / clipRange) * farClip);
        }
    }

    void DirectionalLight::UpdateKDOP(const FrustumPlanes& frustumPlanes, const FrustumCorners& frustumCorners)
    {
        const Vec3 lightDirNormalized = glm::normalize(mLightDirection);

        // Add planes that are facing towards us
        for (const Plane& plane : frustumPlanes) {
            const float fDir = glm::dot(plane.GetNormal(), lightDirNormalized);
            if (fDir < 0.0f) {
                mKDOP.emplace_back(plane);
            }
        }

        // We have added the back sides of the planes.  Now find the edges.
        // For each plane.
        for (uint32_t index = 0; index < frustumPlanes.size(); index++) {
            const Plane& plane = frustumPlanes.at(index);
            // If this plane is facing away from us, move on.
            const float fDir = glm::dot(plane.GetNormal(), lightDirNormalized);
            if (fDir > 0.0f) { continue; }

            // For each neighbor of this plane.
            const auto& neightbourPlanes = GetNeighbouringPlanes(static_cast<FrustumPlane>(index));
            for (const FrustumPlane plane : neightbourPlanes) {
                const float fNeighborDir = glm::dot(frustumPlanes[plane].GetNormal(), lightDirNormalized);
                // If this plane is facing away from us, the edge between plane I and plane J
                //	marks the edge of a plane we need to add.
                if (fNeighborDir > 0.0f) {
                    const auto& cornersOfPlanes = GetCornersOfPlanes(static_cast<FrustumPlane>(index), plane);

                    mKDOP.emplace_back(Vec3(frustumCorners[cornersOfPlanes.first]), Vec3(frustumCorners[cornersOfPlanes.second]), Vec3(frustumCorners[cornersOfPlanes.first]) - lightDirNormalized);
                }
            }
        }

        mCascadeKDOPRange.push_back(mKDOP.size());
    }
}