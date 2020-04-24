#pragma once

#include "Core/Types.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Frustum.h"
#include "Core/Math/MathUtils.h"
#include "Core/Containers/RangedIterator.hpp"
#include "Core/Containers/IDMap.hpp"

#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class DirectionalLight
    {
    public:
        typedef ConstRangedIterator<KDOP> BoundingVolume;

        DirectionalLight(const std::string& name, const uint32_t numShadowmapCascades);

        void SetLightDirection(const Vec3& direction);
        void SetLightColor(const Vec4& lightColor);

        void SetCascadeSplitLambda(const float lambda);
        void UpdateCascadesBoundingVolume(const Mat4& viewMatrix, const float degreesFOV, const float aspectRatio, const float minDepth, const float maxDepth, float zNear, float zFar );
        
        BoundingVolume GetBoundingVolume(const uint32_t cascadeIndex) const;
        void GetSplitDistance(const uint32_t cascadeIndex, float& nearZ, float& farZ) const;

        const std::string& GetName() const;
        uint32_t GetNumCascades() const;
        const Vec4& GetLightColor() const;
        const Vec3& GetLightDirection() const;


	private:
        void UpdateSplitDistances( const float minDepth, const float maxDepth, float zNear, float zFar );
        void UpdateKDOP(const FrustumPlanes& frustumPlanes, const FrustumCorners& frustumCorners);


        std::string mName;
        uint32_t mNumShadowmapCascades;
        Vec4 mLightColor;
        Vec3 mLightDirection;

        KDOP mKDOP;
        std::vector<size_t> mCascadeKDOPRange;
        std::vector<float> mSplitDistances;
        float mCascadeSplitLambda;
    };

    typedef IDMap<DirectionalLight>::ItemID DirectionalLightID;
    static const DirectionalLightID INVALID_DIRECTIONAL_LIGHT_ID = IDMap<DirectionalLight>::INVALID_ITEM_ID;
}