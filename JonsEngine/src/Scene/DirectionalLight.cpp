#include "include/Scene/DirectionalLight.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Math/Math.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    DirectionalLight::DirectionalLight(const std::string& name, const uint32_t numShadowmapCascades) :
        mName(name), mNumShadowmapCascades(numShadowmapCascades), mLightColor(1.0f), mLightDirection(0.0f, -1.0f, -1.0f), mCascadeSplitLambda(0.8f), mHashedID(boost::hash_value(name))
    {
        assert(numShadowmapCascades >= 1);

        mCascadeKDOPRange.reserve(numShadowmapCascades);
        mSplitDistances.reserve(numShadowmapCascades);
    }


    bool DirectionalLight::operator==(const DirectionalLight& light)
    {
        return mHashedID == light.mHashedID;
    }

    bool DirectionalLight::operator==(const std::string& lightName)
    {
        return mHashedID == boost::hash_value(lightName);
    }


    /*
    
    template <uint32_t MAX_KDOP_PLANES>
    void MakeKDop(KDOP<MAX_KDOP_PLANES>& kdop, const std::array<Plane, 6>& frustumPlanes, const FrustumCorners& frustumCorners, const Vec3& lightDir) {
        // We have added the back sides of the planes.  Now find the edges.
        // For each plane.
        for (uint32_t index = 0; index < frustumPlanes.size(); index++) {
            const Plane& plane = frustumPlanes.at(index);
            // If this plane is facing away from us, move on.
            float fDir = glm::dot(plane.mNormal, lightDir);
            if (fDir > 0.0f) { continue; }
            // For each neighbor of this plane.
            LSM_FRUSTUM_PLANES fpNeighbors[4];
            GetNeighbors(index, fpNeighbors);
            for (uint32_t J = 4UL; J--;) {
                float fNeighborDir = glm::dot(frustumPlanes[fpNeighbors[J]].mNormal, lightDir);
                // If this plane is facing away from us, the edge between plane I and plane J
                //	marks the edge of a plane we need to add.
                if (fNeighborDir > 0.0f) {
                    LSM_FRUSTUM_POINTS fpPoints[2];
                    GetCornersOfPlanes(static_cast<LSM_FRUSTUM_PLANES>(index), fpNeighbors[J], fpPoints);
                    Plane pAddMe(Vec3(frustumCorners[fpPoints[0]]), Vec3(frustumCorners[fpPoints[1]]), Vec3(frustumCorners[fpPoints[0]]) + lightDir);
                    kdop.AddPlane(pAddMe);
                }
            }
        }
    }
    */

    enum LSM_FRUSTUM_PLANES {
    LSM_FP_LEFT,
    LSM_FP_RIGHT,
    LSM_FP_TOP,
    LSM_FP_BOTTOM,
    LSM_FP_NEAR,
    LSM_FP_FAR,
    LSM_FP_TOTAL
    };

    enum LSM_FRUSTUM_POINTS {
    LSM_FP_NEAR_BOTTOM_LEFT,
    LSM_FP_NEAR_TOP_LEFT,
    LSM_FP_NEAR_TOP_RIGHT,
    LSM_FP_NEAR_BOTTOM_RIGHT,
    LSM_FP_FAR_BOTTOM_LEFT,
    LSM_FP_FAR_TOP_LEFT,
    LSM_FP_FAR_TOP_RIGHT,
    LSM_FP_FAR_BOTTOM_RIGHT,
    };

    void GetNeighbors(uint32_t index, LSM_FRUSTUM_PLANES _fpRet[4]) {
    static const LSM_FRUSTUM_PLANES fpTable[LSM_FP_TOTAL][4] = {
    {	// LSM_FP_LEFT
    LSM_FP_TOP,
    LSM_FP_BOTTOM,
    LSM_FP_NEAR,
    LSM_FP_FAR
    },
    {	// LSM_FP_RIGHT
    LSM_FP_TOP,
    LSM_FP_BOTTOM,
    LSM_FP_NEAR,
    LSM_FP_FAR
    },
    {	// LSM_FP_TOP
    LSM_FP_LEFT,
    LSM_FP_RIGHT,
    LSM_FP_NEAR,
    LSM_FP_FAR
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_LEFT,
    LSM_FP_RIGHT,
    LSM_FP_NEAR,
    LSM_FP_FAR
    },
    {	// LSM_FP_NEAR
    LSM_FP_LEFT,
    LSM_FP_RIGHT,
    LSM_FP_TOP,
    LSM_FP_BOTTOM
    },
    {	// LSM_FP_FAR
    LSM_FP_LEFT,
    LSM_FP_RIGHT,
    LSM_FP_TOP,
    LSM_FP_BOTTOM
    },
    };

    for (uint32_t I = 4UL; I--;) {
    _fpRet[I] = fpTable[index][I];
    }
    }


    void GetCornersOfPlanes(LSM_FRUSTUM_PLANES _fpPlane0, LSM_FRUSTUM_PLANES _fpPlane1, LSM_FRUSTUM_POINTS _fpRet[2]) {
    static const LSM_FRUSTUM_POINTS fpTable[LSM_FP_TOTAL][LSM_FP_TOTAL][2] = {
    {	// LSM_FP_LEFT
    {	// LSM_FP_LEFT
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
    },
    {	// LSM_FP_RIGHT
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
    },
    {	// LSM_FP_TOP
    LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
    },
    {	// LSM_FP_NEAR
    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_TOP_LEFT,
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
    },
    },
    {	// LSM_FP_RIGHT
    {	// LSM_FP_LEFT
    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,	// Invalid combination.
    },
    {	// LSM_FP_RIGHT
    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,	// Invalid combination.
    },
    {	// LSM_FP_TOP
    LSM_FP_FAR_TOP_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
    },
    {	// LSM_FP_NEAR
    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_TOP_RIGHT,
    },
    },


    {	// LSM_FP_TOP
    {	// LSM_FP_LEFT
    LSM_FP_FAR_TOP_LEFT, LSM_FP_NEAR_TOP_LEFT,
    },
    {	// LSM_FP_RIGHT
    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_FAR_TOP_RIGHT,
    },
    {	// LSM_FP_TOP
    LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
    },
    {	// LSM_FP_NEAR
    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,
    },
    },
    {	// LSM_FP_BOTTOM
    {	// LSM_FP_LEFT
    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
    },
    {	// LSM_FP_RIGHT
    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
    },
    {	// LSM_FP_TOP
    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,	// Invalid combination.
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
    },
    {	// LSM_FP_NEAR
    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_LEFT,
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_RIGHT,
    },
    },


    {	// LSM_FP_NEAR
    {	// LSM_FP_LEFT
    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
    },
    {	// LSM_FP_RIGHT
    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
    },
    {	// LSM_FP_TOP
    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_TOP_LEFT,
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_RIGHT,
    },
    {	// LSM_FP_NEAR
    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,		// Invalid combination.
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
    },
    },
    {	// LSM_FP_FAR
    {	// LSM_FP_LEFT
    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_TOP_LEFT,
    },
    {	// LSM_FP_RIGHT
    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
    },
    {	// LSM_FP_TOP
    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,
    },
    {	// LSM_FP_BOTTOM
    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_LEFT,
    },
    {	// LSM_FP_NEAR
    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,		// Invalid combination.
    },
    {	// LSM_FP_FAR
    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
    },
    },
    };
    _fpRet[0] = fpTable[_fpPlane0][_fpPlane1][0];
    _fpRet[1] = fpTable[_fpPlane0][_fpPlane1][1];
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


    ConstRangedIterator<KDOP> DirectionalLight::GetBoundingVolume(const uint32_t cascadeIndex) const
    {
        assert(cascadeIndex < mNumShadowmapCascades);

        const size_t startIndex = cascadeIndex == 0 ? 0 : mCascadeKDOPRange.at(cascadeIndex - 1) + 1;
        const size_t endIndex = mCascadeKDOPRange.at(cascadeIndex);

        return ConstRangedIterator<KDOP>(mKDOP, startIndex, endIndex);
    }

    const std::vector<float>& DirectionalLight::GetSplitDistances() const
    {
        return mSplitDistances;
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
            const float fDir = glm::dot(plane.mNormal, lightDirNormalized);
            if (fDir < 0.0f) {
                mKDOP.emplace_back(plane);
            }
        }

        // We have added the back sides of the planes.  Now find the edges.
        // For each plane.
        for (uint32_t index = 0; index < frustumPlanes.size(); index++) {
            const Plane& plane = frustumPlanes.at(index);
            // If this plane is facing away from us, move on.
            float fDir = glm::dot(plane.mNormal, lightDirNormalized);
            if (fDir > 0.0f) { continue; }
            // For each neighbor of this plane.
            LSM_FRUSTUM_PLANES fpNeighbors[4];
            GetNeighbors(index, fpNeighbors);
            for (uint32_t J = 4UL; J--;) {
                float fNeighborDir = glm::dot(frustumPlanes[fpNeighbors[J]].mNormal, lightDirNormalized);
                // If this plane is facing away from us, the edge between plane I and plane J
                //	marks the edge of a plane we need to add.
                if (fNeighborDir > 0.0f) {
                    LSM_FRUSTUM_POINTS fpPoints[2];
                    GetCornersOfPlanes(static_cast<LSM_FRUSTUM_PLANES>(index), fpNeighbors[J], fpPoints);
                    
                    mKDOP.emplace_back(Vec3(frustumCorners[fpPoints[0]]), Vec3(frustumCorners[fpPoints[1]]), Vec3(frustumCorners[fpPoints[0]]) - lightDirNormalized);
                }
            }
        }

        //const size_t kdopEndSize = mKDOP.size();

        mCascadeKDOPRange.push_back(mKDOP.size() - 1);
    }
}