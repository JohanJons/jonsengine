#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF2X2Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF3X3Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF5X5Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF7X7Pixel.h"
#include "include/Core/Math/Math.h"

#include <array>
#include <limits>

namespace JonsEngine
{
    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, -0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.5f, 0.5f, 0.0f, 1.0f);


    // TODO: this could probably be made tighter
    Mat4 CreateDirLightVPMatrix(const FrustumCorners& frustumCorners, const Vec3& lightDir)
    {
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -lightDir, Vec3(0.0f, -1.0f, 0.0f));

        Vec4 transf = lightViewMatrix * frustumCorners[0];
        float maxZ = transf.z, minZ = transf.z;
        float maxX = transf.x, minX = transf.x;
        float maxY = transf.y, minY = transf.y;
        for (uint32_t i = 1; i < 8; ++i)
        {
            transf = lightViewMatrix * frustumCorners[i];

            if (transf.z > maxZ) maxZ = transf.z;
            if (transf.z < minZ) minZ = transf.z;
            if (transf.x > maxX) maxX = transf.x;
            if (transf.x < minX) minX = transf.x;
            if (transf.y > maxY) maxY = transf.y;
            if (transf.y < minY) minY = transf.y;
        }

        lightViewMatrix[3][0] = -(minX + maxX) * 0.5f;
        lightViewMatrix[3][1] = -(minY + maxY) * 0.5f;
        lightViewMatrix[3][2] = -(minZ + maxZ) * 0.5f;

        Vec3 halfExtents((maxX - minX) * 0.5, (maxY - minY) * 0.5, (maxZ - minZ) * 0.5);

        return OrthographicMatrix(-halfExtents.x, halfExtents.x, halfExtents.y, -halfExtents.y, halfExtents.z, -halfExtents.z) * lightViewMatrix;
    }


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const RenderSettings::ShadowResolution shadowmapRes,
        const RenderSettings::ShadowReadbackLatency readbackLatency, const uint32_t windowWidth, const uint32_t windowHeight)
        :
        mWindowSize(windowWidth, windowHeight),
        mAspectRatio(mWindowSize.x / mWindowSize.y),

        mContext(context),

        mPCF2x2Shader(nullptr),
        mPCF3x3Shader(nullptr),
        mPCF5x5Shader(nullptr),
        mPCF7x7Shader(nullptr),

        mRSDepthClamp(nullptr),
        mFullscreenPass(fullscreenPass),
        mVertexTransformPass(transformPass),
        mShadowmap(device, context, shadowmapRes, NUM_SHADOWMAP_CASCADES, false),
        mDirLightCBuffer(device, context, mDirLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF2X2PixelShader, sizeof(gDirectionalLightPCF2X2PixelShader), nullptr, &mPCF2x2Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF3X3PixelShader, sizeof(gDirectionalLightPCF3X3PixelShader), nullptr, &mPCF3x3Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF5X5PixelShader, sizeof(gDirectionalLightPCF5X5PixelShader), nullptr, &mPCF5x5Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF7X7PixelShader, sizeof(gDirectionalLightPCF7X7PixelShader), nullptr, &mPCF7x7Shader));

        // depth clamp to avoid meshes between frustrum split issues
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.SlopeScaledDepthBias = 1.0f;
        rasterizerDesc.DepthBiasClamp = 1.0f;
        rasterizerDesc.DepthClipEnable = false;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSDepthClamp));
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
    }


    void DX11DirectionalLightPass::Render(const RenderableDirectionalLight& directionalLight, const RenderQueue::RenderData& renderData, const RenderSettings::ShadowFiltering shadowFiltering, const float degreesFOV, const Mat4& cameraViewMatrix)
    {
        // preserve current state
        D3D11_VIEWPORT prevViewport;
        ID3D11RasterizerStatePtr prevRS = nullptr;
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        ID3D11DepthStencilStatePtr prevDSState = nullptr;
        uint32_t numViewports = 1;
        mContext->RSGetViewports(&numViewports, &prevViewport);
        mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);
        mContext->OMGetDepthStencilState(&prevDSState, 0);
        mContext->RSGetState(&prevRS);


        //
        // Shadow pass
        //

        // unbind any set pixel shader
        mContext->PSSetShader(nullptr, nullptr, 0);

        // depth clamp to avoid issues with meshes between splits
        mContext->RSSetState(mRSDepthClamp);

        mShadowmap.BindForDrawing();

        const float maxFloatVal = std::numeric_limits<float>::max();
        std::array<float, NUM_SHADOWMAP_CASCADES> splitDistances = { maxFloatVal, maxFloatVal, maxFloatVal, maxFloatVal };
        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;
        for (uint32_t cascadeIndex = 0; cascadeIndex < directionalLight.mNumCascadesUsed; ++cascadeIndex)
        {
            mShadowmap.BindDepthView(cascadeIndex);

            const auto& cascadeSplit = directionalLight.mCascades.at(cascadeIndex);
            
            // create view-projection matrix for cascade
            const Mat4 perspectiveMatrix = PerspectiveMatrixFov(degreesFOV, mAspectRatio, cascadeSplit.mNearZ, cascadeSplit.mFarZ);
            const auto frustumCorners = GetFrustumCorners(perspectiveMatrix * cameraViewMatrix);
            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(frustumCorners, directionalLight.mDirection);

            // render meshes from lights POV
            //const size_t meshStartIndex = cascadeIndex == 0 ? 0 : directionalLight.mCascadeSplits.at(cascadeIndex - 1).mMeshEndIndex;
            //const size_t meshEndIndex = cascadeSplit.mMeshEndIndex;
            //ConstRangedIterator<RenderableMeshes> meshIterator(directionalLight.mMeshes, meshStartIndex, meshEndIndex);
			const Mat4& lightViewProj = lightVPMatrices[cascadeIndex];
			mVertexTransformPass.RenderMeshes(renderData, cascadeSplit, lightViewProj);

            // store view-projection for shading pass
            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex] * glm::inverse(cameraViewMatrix);
        }

        // negate the split distances for comparison against view space position in the shader
        for (uint32_t cascadeIndex = 0; cascadeIndex < directionalLight.mNumCascadesUsed; ++cascadeIndex)
            splitDistances[cascadeIndex] = -directionalLight.mCascades.at(cascadeIndex).mFarZ;


        //
        // Shading pass
        //
        
        // restore state
        mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mContext->RSSetViewports(numViewports, &prevViewport);
        mContext->RSSetState(prevRS);
        mContext->OMSetDepthStencilState(prevDSState, 0);
        
        // bind shadowmap SRV for reading
        mShadowmap.BindForReading();

        const Vec4 camLightDir = glm::normalize(cameraViewMatrix * Vec4(-directionalLight.mDirection, 0));

        // set dir light cbuffer data
        mDirLightCBuffer.SetData(DirectionalLightCBuffer(lightVPMatrices, splitDistances, directionalLight.mColor, camLightDir, mWindowSize, static_cast<float>(mShadowmap.GetTextureSize())));
		mDirLightCBuffer.Bind();

        // bind appropiate shading pixel shader for shadow filtering argument
        BindShadingPixelShader(shadowFiltering);

        // run fullscreen pass + dir light shading pass
        mFullscreenPass.Render();
    }


    void DX11DirectionalLightPass::BindShadingPixelShader(const RenderSettings::ShadowFiltering shadowFiltering)
    {
        switch (shadowFiltering)
        {
            default:
            case RenderSettings::ShadowFiltering::PCF_2x2:
            {
                mContext->PSSetShader(mPCF2x2Shader, nullptr, 0);
                break;
            }
            case RenderSettings::ShadowFiltering::PCF_3x3:
            {
                mContext->PSSetShader(mPCF3x3Shader, nullptr, 0);
                break;
            }
            case RenderSettings::ShadowFiltering::PCF_5x5:
            {
                mContext->PSSetShader(mPCF5x5Shader, nullptr, 0);
                break;
            }
            case RenderSettings::ShadowFiltering::PCF_7x7:
            {
                mContext->PSSetShader(mPCF7x7Shader, nullptr, 0);
                break;
            }
        }
    }
}