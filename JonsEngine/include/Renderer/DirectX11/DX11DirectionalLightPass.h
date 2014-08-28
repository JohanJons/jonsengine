#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <array>

namespace JonsEngine
{
    class DX11Backbuffer;
    class DX11FullscreenTrianglePass;
    class DX11VertexTransformPass;

    class DX11DirectionalLightPass
    {
    public:
        const static uint32_t NUM_SHADOWMAP_CASCADES = 4;

        DX11DirectionalLightPass(ID3D11DevicePtr device, DX11Backbuffer& backbuffer, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, uint32_t shadowmapSize);
        ~DX11DirectionalLightPass();

        void BindForShading(ID3D11DeviceContextPtr context);
        void Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix, const Vec4& lightColor, const Vec3& lightDir, const bool drawFrustrums);


    private:
        struct DirectionalLightCBuffer
        {
            std::array<Mat4, NUM_SHADOWMAP_CASCADES> mSplitVPMatrices;
            Mat4 mCameraViewMatrix;
            std::array<float, NUM_SHADOWMAP_CASCADES> mSplitDistances;
            Vec4 mLightColor;
            Vec4 mLightDirection;


            DirectionalLightCBuffer(const std::array<Mat4, NUM_SHADOWMAP_CASCADES>& splitMatrices, const Mat4& cameraViewMatrix, const std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances, const Vec4& lightColor, const Vec3& lightDir) :
                mSplitVPMatrices(splitMatrices), mCameraViewMatrix(cameraViewMatrix), mSplitDistances(splitDistances), mLightColor(lightColor), mLightDirection(Vec4(-lightDir, 0.0f))
            {
            }
        };

        ID3D11PixelShaderPtr mPixelShader;
        ID3D11RasterizerStatePtr mRSDepthClamp;
        DX11Backbuffer& mBackbuffer;
        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<DirectionalLightCBuffer> mDirLightCBuffer;
    };
}
