#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMInitialCompute.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMFinalCompute.h"
#include "include/Core/Utils/Math.h"

#include <array>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const uint32_t gSDSMThreadGroupSize = 16;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, -0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.5f, 0.5f, 0.0f, 1.0f);

    void CalculateShadowmapCascades(std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& nearDistArr, std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& farDistArr, const float nearDist, const  float farDist)
    {
        const float splitWeight = 0.75f;
        const float ratio = nearDist / farDist;

        nearDistArr[0] = nearDist;
        for (uint8_t index = 1; index < DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES; index++)
        {
            const float si = index / (float)DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES;

            nearDistArr[index] = splitWeight * (nearDist * powf(ratio, si)) + (1 - splitWeight) * (nearDist + (farDist - nearDist) * si);
            farDistArr[index - 1] = nearDistArr[index] * 1.005f;
        }
        farDistArr[DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES - 1] = farDist;
    }

    CameraFrustrum CalculateCameraFrustrum(const float fovDegrees, const float aspectRatio, const float minDist, const float maxDist, const Mat4& cameraViewMatrix)
    {
        CameraFrustrum ret = { Vec4(1.0f, -1.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
                               Vec4(1.0f, -1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, -1.0f, 1.0f, 1.0f), };

        const Mat4 perspectiveMatrix = PerspectiveMatrixFov(fovDegrees, aspectRatio, minDist, maxDist);
        const Mat4 invMVP = glm::inverse(perspectiveMatrix * cameraViewMatrix);

        for (Vec4& corner : ret)
        {
            corner = invMVP * corner;
            corner /= corner.w;
        }

        return ret;
    }

    Mat4 CreateDirLightVPMatrix(const CameraFrustrum& cameraFrustrum, const Vec3& lightDir)
    {
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -glm::normalize(lightDir), Vec3(0.0f, -1.0f, 0.0f));

        Vec4 transf = lightViewMatrix * cameraFrustrum[0];
        float maxZ = transf.z, minZ = transf.z;
        float maxX = transf.x, minX = transf.x;
        float maxY = transf.y, minY = transf.y;
        for (uint32_t i = 1; i < 8; i++)
        {
            transf = lightViewMatrix * cameraFrustrum[i];

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

    // Computes a compute shader dispatch size given a thread group size, and number of elements to process
    uint32_t DispatchSize(uint32_t tgSize, uint32_t numElements)
    {
        uint32_t dispatchSize = numElements / tgSize;
        dispatchSize += numElements % tgSize > 0 ? 1 : 0;

        return dispatchSize;
    }


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const uint32_t shadowmapSize,
        const uint32_t windowWidth, const uint32_t windowHeight)
        :
        mContext(context),
        mPixelShader(nullptr),
        mSDSMInitialShader(nullptr),
        mSDSMFinalShader(nullptr),
        mRSDepthClamp(nullptr),
        mFullscreenPass(fullscreenPass),
        mVertexTransformPass(transformPass),
        mShadowmap(device, context, shadowmapSize, NUM_SHADOWMAP_CASCADES, false),
        mDirLightCBuffer(device, context, mDirLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mSDSMCBuffer(device, context, mSDSMCBuffer.CONSTANT_BUFFER_SLOT_COMPUTE)
    {
        DXCALL(device->CreatePixelShader(gDirectionalLightPixelShader, sizeof(gDirectionalLightPixelShader), nullptr, &mPixelShader));
        DXCALL(device->CreateComputeShader(gSDSMInitialComputeShader, sizeof(gSDSMInitialComputeShader), nullptr, &mSDSMInitialShader));
        DXCALL(device->CreateComputeShader(gSDSMFinalComputeShader, sizeof(gSDSMFinalComputeShader), nullptr, &mSDSMFinalShader));

        // depth clamp to avoid meshes between frustrum split issues
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthBiasClamp = 1.0f;
        rasterizerDesc.DepthClipEnable = false;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSDepthClamp));

        // setup UAVs for compute shader
        uint32_t width = windowWidth;
        uint32_t height = windowHeight;
        while (width > 1 || height > 1)
        {
            width = DispatchSize(gSDSMThreadGroupSize, width);
            height = DispatchSize(gSDSMThreadGroupSize, height);

            mDepthReductionRTVs.emplace_back(device, DXGI_FORMAT_R16G16_UNORM, width, height, true);
        }
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
    }


    void DX11DirectionalLightPass::Render(const RenderableDirLight& directionalLight, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix, const Mat4& invCameraProjMatrix, const Vec2& windowSize, const Mat4& cameraProjMatrix)
    {
        // preserve current state
        D3D11_VIEWPORT prevViewport;
        ID3D11RasterizerStatePtr prevRS = nullptr;
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        uint32_t numViewports = 1;
        mContext->RSGetViewports(&numViewports, &prevViewport);
        mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);
        mContext->RSGetState(&prevRS);


        //
        // Reduce Depth pass
        //

        // near/far z range
        Vec2 zRange = ReduceDepth(cameraProjMatrix);

        zRange.x = Z_NEAR;
        zRange.y = Z_FAR;


        //
        // Shadow pass
        //

        // unbind any set pixel shader
        mContext->PSSetShader(nullptr, nullptr, 0);

        // depth clamp to avoid issues with meshes between splits
        mContext->RSSetState(mRSDepthClamp);

        std::array<float, NUM_SHADOWMAP_CASCADES> nearDistArr, farDistArr;
        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;

        // TODO: precompute?
        CalculateShadowmapCascades(nearDistArr, farDistArr, zRange.x, zRange.y);

        mVertexTransformPass.BindForTransformPass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mShadowmap.BindForDrawing();
        
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; cascadeIndex++)
        {
            mShadowmap.BindDepthView(cascadeIndex);
            
            CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(degreesFOV, aspectRatio, nearDistArr[cascadeIndex], farDistArr[cascadeIndex], cameraViewMatrix);
            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, directionalLight.mLightDirection);
			mVertexTransformPass.RenderMeshes(directionalLight.mMeshes, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex] * glm::inverse(cameraViewMatrix);
            farDistArr[cascadeIndex] = -farDistArr[cascadeIndex];
        }


        //
        // Shading pass
        //
        
        // restore rendering to backbuffer, rasterize state and viewport
        mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mContext->RSSetViewports(numViewports, &prevViewport);
        mContext->RSSetState(prevRS);
        
        // bind shadowmap SRV for reading
        mShadowmap.BindForReading();

        const Vec4 camLightDir = glm::normalize(cameraViewMatrix * Vec4(-directionalLight.mLightDirection, 0));

        // set dir light cbuffer data and pixel shader
		mDirLightCBuffer.SetData(DirectionalLightCBuffer(lightVPMatrices, invCameraProjMatrix, farDistArr, directionalLight.mLightColor, camLightDir, windowSize, static_cast<float>(mShadowmap.GetTextureSize())));
        mContext->PSSetShader(mPixelShader, nullptr, 0);

        // run fullscreen pass + dir light shading pass
        mFullscreenPass.Render();
    }


    Vec2 DX11DirectionalLightPass::ReduceDepth(const Mat4& cameraProjMatrix)
    {
        mContext->OMSetRenderTargets(0, nullptr, nullptr);

		// first pass
        mSDSMCBuffer.SetData(SDSMCBuffer(cameraProjMatrix[2].z, cameraProjMatrix[3].z, 0.1f, 100.0f));

        auto& initialRTV = mDepthReductionRTVs.front();
		mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &initialRTV.mUAV.p, nullptr);
        mContext->CSSetShader(mSDSMInitialShader, nullptr, 0);

		D3D11_TEXTURE2D_DESC rtvTextureDesc;
		ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		initialRTV.mTexture->GetDesc(&rtvTextureDesc);

		mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);

		// subsequent passes
		mContext->CSSetShader(mSDSMFinalShader, nullptr, 0);
		for (uint32_t index = 1; index < mDepthReductionRTVs.size(); index++)
		{
			auto& prevRTV = mDepthReductionRTVs.at(index - 1);
			mContext->CSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &prevRTV.mSRV);

			auto& rtv = mDepthReductionRTVs.at(index);
            mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &rtv.mUAV, nullptr);

			ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			rtv.mTexture->GetDesc(&rtvTextureDesc);

			mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);
		}

        mContext->CSSetUnorderedAccessViews(UAV_SLOT, 0, nullptr, nullptr);
        mContext->CSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 0, nullptr);

        // TODO
        return Vec2(0.1f, 1.0f);
    }
}