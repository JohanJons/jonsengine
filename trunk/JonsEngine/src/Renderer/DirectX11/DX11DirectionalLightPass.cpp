#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMInitialCompute.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SDSMFinalCompute.h"
#include "include/Core/Utils/Math.h"

#include <array>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, -0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.5f, 0.5f, 0.0f, 1.0f);

    /*void CalculateShadowmapCascades(std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& nearDistArr, std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& farDistArr, const float nearDist, const  float farDist)
    {
        const float splitWeight = 0.75f;
        const float ratio = nearDist / farDist;

        nearDistArr[0] = nearDist;
        for (uint8_t index = 1; index < DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES; ++index)
        {
            const float si = index / (float)DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES;

            nearDistArr[index] = splitWeight * (nearDist * powf(ratio, si)) + (1 - splitWeight) * (nearDist + (farDist - nearDist) * si);
            farDistArr[index - 1] = nearDistArr[index] * 1.005f;
        }
        farDistArr[DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES - 1] = farDist;
    }*/

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
        for (uint32_t i = 1; i < 8; ++i)
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


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const EngineSettings::ShadowResolution shadowmapRes,
        const EngineSettings::ShadowReadbackLatency readbackLatency, const uint32_t windowWidth, const uint32_t windowHeight)
        :
        mReadbackLatency(readbackLatency),
        mCurrFrame(0),

        mContext(context),
        mPixelShader(nullptr),
        mSDSMInitialShader(nullptr),
        mSDSMFinalShader(nullptr),
        mRSDepthClamp(nullptr),

        mFullscreenPass(fullscreenPass),
        mVertexTransformPass(transformPass),
        mShadowmap(device, context, shadowmapRes, NUM_SHADOWMAP_CASCADES, false),
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
			width = DispatchSize(SDSM_THREAD_GROUP_SIZE, width);
			height = DispatchSize(SDSM_THREAD_GROUP_SIZE, height);

            mDepthReductionRTVs.emplace_back(device, DXGI_FORMAT_R16G16_UNORM, width, height, true);
        }

        // setup readback textures
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.ArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Format = DXGI_FORMAT_R16G16_UNORM;
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        for (uint32_t index = 0; index < MAX_READBACK_LATENCY; ++index)
            DXCALL(device->CreateTexture2D(&textureDesc, nullptr, &mReadbackTextures[index]));
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

		std::array<float, NUM_SHADOWMAP_CASCADES> splitDistances;
		const float minZ = CalculateShadowmapCascades(cameraProjMatrix, splitDistances);


        //
        // Shadow pass
        //

        // unbind any set pixel shader
        mContext->PSSetShader(nullptr, nullptr, 0);

        // depth clamp to avoid issues with meshes between splits
        mContext->RSSetState(mRSDepthClamp);

        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;

        mVertexTransformPass.BindForTransformPass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mShadowmap.BindForDrawing();
        
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
        {
            mShadowmap.BindDepthView(cascadeIndex);
            
			CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(degreesFOV, aspectRatio, cascadeIndex == 0 ? minZ : splitDistances[cascadeIndex - 1], splitDistances[cascadeIndex], cameraViewMatrix);
            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, directionalLight.mLightDirection);
			mVertexTransformPass.RenderMeshes(directionalLight.mMeshes, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex] * glm::inverse(cameraViewMatrix);
        }

        // negate the split distances for comparison against view space position in the shader
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
            splitDistances[cascadeIndex] = -splitDistances[cascadeIndex];


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
		mDirLightCBuffer.SetData(DirectionalLightCBuffer(lightVPMatrices, invCameraProjMatrix, splitDistances, directionalLight.mLightColor, camLightDir, windowSize, static_cast<float>(mShadowmap.GetTextureSize())));
        mContext->PSSetShader(mPixelShader, nullptr, 0);

        // run fullscreen pass + dir light shading pass
        mFullscreenPass.Render();
    }


	float DX11DirectionalLightPass::CalculateShadowmapCascades(const Mat4& cameraProjMatrix, std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances)
	{
		float minDepth, maxDepth;
		ReduceDepth(cameraProjMatrix, minDepth, maxDepth);

		const float nearClip = Z_NEAR;
		const float farClip = Z_FAR;
		const float clipRange = farClip - nearClip;

		const float minZ = nearClip + minDepth * clipRange;
		const float maxZ = nearClip + maxDepth * clipRange;

        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

		for (uint32_t cascadeIndex = 0; cascadeIndex < DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
		{
			const float p = (cascadeIndex + 1) / static_cast<float>(DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES);
			const float log = minZ * std::pow(ratio, p);
			const float uniform = minZ + range * p;
			const float d = log - uniform + uniform;
			splitDistances[cascadeIndex] = (d - nearClip) / clipRange;

            splitDistances[cascadeIndex] *= farClip;
		}

		// minZ is used for first splits near distance
		return minZ;
	}

	void DX11DirectionalLightPass::ReduceDepth(const Mat4& cameraProjMatrix, float& minDepth, float& maxDepth)
    {
        mContext->OMSetRenderTargets(0, nullptr, nullptr);

		// first pass
        // TODO: why neg. [2].z?
        mSDSMCBuffer.SetData(SDSMCBuffer(-cameraProjMatrix[2].z, cameraProjMatrix[3].z, 0.1f, 100.0f));

        auto& initialRTV = mDepthReductionRTVs.front();
		mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &initialRTV.mUAV.p, nullptr);
        mContext->CSSetShader(mSDSMInitialShader, nullptr, 0);

		D3D11_TEXTURE2D_DESC rtvTextureDesc;
		ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		initialRTV.mTexture->GetDesc(&rtvTextureDesc);

		mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);

		mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &gNullUAV.p, nullptr);

		// subsequent passes
		mContext->CSSetShader(mSDSMFinalShader, nullptr, 0);
		for (uint32_t index = 1; index < mDepthReductionRTVs.size(); ++index)
		{
			auto& prevRTV = mDepthReductionRTVs.at(index - 1);
			mContext->CSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &prevRTV.mSRV.p);

			auto& rtv = mDepthReductionRTVs.at(index);
            mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &rtv.mUAV.p, nullptr);

			ZeroMemory(&rtvTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			rtv.mTexture->GetDesc(&rtvTextureDesc);

			mContext->Dispatch(rtvTextureDesc.Width, rtvTextureDesc.Height, 1);

			mContext->CSSetUnorderedAccessViews(UAV_SLOT, 1, &gNullUAV.p, nullptr);
			mContext->CSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &gNullSRV.p);
		}

        // reading back depth
        const uint32_t latency = EngineSettingsToVal(mReadbackLatency) + 1;
        ID3D11Texture2DPtr lastTarget = mDepthReductionRTVs[mDepthReductionRTVs.size() - 1].mTexture;
        mContext->CopyResource(mReadbackTextures[mCurrFrame % latency].p, lastTarget.p);

		// note: when this overflows, will cause a few bad frames
        ++mCurrFrame;
        if (mCurrFrame >= latency)
        {
            ID3D11Texture2DPtr readbackTexture = mReadbackTextures[mCurrFrame % latency];

            D3D11_MAPPED_SUBRESOURCE mapped;
            DXCALL(mContext->Map(readbackTexture.p, 0, D3D11_MAP_READ, 0, &mapped));
            mContext->Unmap(readbackTexture.p, 0);

            const uint16_t* texData = reinterpret_cast<uint16_t*>(mapped.pData);

			minDepth = texData[0] / static_cast<float>(0xffff);
			maxDepth = texData[1] / static_cast<float>(0xffff);
        }
		else
		{
			minDepth = 0.01f;
			maxDepth = 1.0f;
		}
    }
}