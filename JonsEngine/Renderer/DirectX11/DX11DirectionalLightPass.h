#pragma once

#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11Mesh.h"
#include "Renderer/DirectX11/DX11Shadowmap.h"
#include "RenderQueue/RenderQueue.h"
#include "Core/Types.h"
#include "Core/Math/Frustum.h"
#include "Core/EngineSettings.h"
#include "Core/Platform.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11VertexTransformPass;
	struct RenderableDirectionalLight;

    class DX11DirectionalLightPass
    {
    public:
        const static uint32_t NUM_SHADOWMAP_CASCADES = 4;

        DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const RenderSettings::ShadowResolution shadowmapRes,
            const RenderSettings::ShadowReadbackLatency readbackLatency);
        ~DX11DirectionalLightPass();

        void Render(const RenderableDirectionalLight& directionalLight, const RenderData& renderData, RenderSettings::ShadowFiltering shadowFiltering, float degreesFOV, float aspectRatio, const Mat4& cameraViewMatrix);


    private:
        struct DirectionalLightCBuffer
        {
            std::array<Mat4, NUM_SHADOWMAP_CASCADES> mSplitVPMatrices;
            std::array<float, NUM_SHADOWMAP_CASCADES> mSplitDistances;
            Vec4 mLightColor;
            Vec4 mLightDirection;
            float mShadowmapSize;
            float __padding[ 3 ];


            DirectionalLightCBuffer(const std::array<Mat4, NUM_SHADOWMAP_CASCADES>& splitMatrices, const std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances, const Vec4& lightColor, const Vec4& lightDir, const float shadowmapSize) :
                mSplitVPMatrices(splitMatrices), mSplitDistances(splitDistances), mLightColor(lightColor), mLightDirection(lightDir), mShadowmapSize(shadowmapSize)
            {
            }
        };

        void BindShadingPixelShader(const RenderSettings::ShadowFiltering shadowFiltering);


        ID3D11DeviceContextPtr mContext;

        ID3D11PixelShaderPtr mPCF2x2Shader;
        ID3D11PixelShaderPtr mPCF3x3Shader;
        ID3D11PixelShaderPtr mPCF5x5Shader;
        ID3D11PixelShaderPtr mPCF7x7Shader;

        ID3D11RasterizerStatePtr mRSDepthClamp;
        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<DirectionalLightCBuffer> mDirLightCBuffer;
    };
}
