#include "include/Renderer/DirectX11/DX11Pipeline.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    const DX11Color gClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Pipeline::DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, const EngineSettings::ShadowResolution shadowmapResolution,
        const EngineSettings::ShadowReadbackLatency shadowmapReadbackLatency, IDMap<DX11Mesh>& meshMap, IDMap<DX11Material>& materialMap)
        :
        mLogger(logger),
        mWindowSize(backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mMeshMap(meshMap),
        mMaterialMap(materialMap),

        mSwapchain(swapchain),
        mContext(context),
        mDSV(nullptr),
        mDSVReadOnly(nullptr),
        mDepthSRV(nullptr),
        mDepthStencilState(nullptr),
        mAdditiveBlending(nullptr),

        mBoneTransformsBuffer(device, context),
        mVertexTransformPass(device, context, meshMap),
        mAABBPass(device, context, mVertexTransformPass),
        mFullscreenTrianglePass(device, context),

        mBackbuffer(device, mContext, mSwapchain, mFullscreenTrianglePass),
        mLightAccbuffer(device, mContext, backbufferTextureDesc),
        mGBuffer(device, mContext, backbufferTextureDesc),

        mAmbientPass(device, context, mFullscreenTrianglePass, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mDirectionalLightPass(device, mContext, mFullscreenTrianglePass, mVertexTransformPass, shadowmapResolution, shadowmapReadbackLatency, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mPointLightPass(device, mContext, mVertexTransformPass, shadowmapResolution, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mPostProcessor(device, context, mFullscreenTrianglePass, backbufferTextureDesc),
        mSkyboxPass(device, context)
    {
        // create depth buffer/view/srv
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilBufferDesc.Width = backbufferTextureDesc.Width;
        depthStencilBufferDesc.Height = backbufferTextureDesc.Height;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &mDepthStencilBuffer));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(mDepthStencilBuffer, &srvDesc, &mDepthSRV));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSV));

        dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSVReadOnly));

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

        // create blend state for shading pass
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        DXCALL(device->CreateBlendState(&blendDesc, &mAdditiveBlending));
    }

    DX11Pipeline::~DX11Pipeline()
    {
    }


    void DX11Pipeline::BeginFrame(const RenderQueue& renderQueue)
    {
        mBackbuffer.ClearBackbuffer(gClearColor);

		// Send all bone transforms to GPU
		const bool hasBoneData = !renderQueue.mRenderData.mBones.empty();
		if (hasBoneData)
		{
			mBoneTransformsBuffer.SetData(renderQueue.mRenderData.mBones);
			mBoneTransformsBuffer.Bind(DX11DynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_BONE_TRANSFORMS);
		}
    }

    void DX11Pipeline::EndFrame()
    {
        DXCALL(mSwapchain->Present(0, 0));
    }


    void DX11Pipeline::GeometryStage(const RenderQueue& renderQueue)
    {
        mGBuffer.BindForGeometryStage(mDSV);

		const auto staticBeginIndex = renderQueue.mCamera.mStaticMeshesBegin;
		const auto staticEndIndex = renderQueue.mCamera.mStaticMeshesEnd;
		const auto& staticMeshesContainer = renderQueue.mRenderData.mStaticMeshes;
		RenderMeshes(renderQueue, staticMeshesContainer, staticBeginIndex, staticEndIndex);

		// TEMP SOLUTION !!!
		// STATICS
		/*const auto staticBeginIndex = renderQueue.mCamera.mStaticMeshesBegin;
		const auto staticEndIndex = renderQueue.mCamera.mStaticMeshesEnd;
		for (auto meshIndex = staticBeginIndex; meshIndex < staticEndIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = renderQueue.mRenderData.mStaticMeshes.at(meshIndex);
			assert(mesh.mMeshID != INVALID_DX11_MESH_ID);

			bool hasDiffuseTexture = false, hasNormalTexture = false;
			if (mesh.mMaterial != RenderableMaterial::INVALID_INDEX)
			{
				const RenderableMaterial& material = renderQueue.mRenderData.mMaterials.at(mesh.mMaterial);
				hasDiffuseTexture = material.mDiffuseTextureID != INVALID_DX11_MATERIAL_ID;
				hasNormalTexture = material.mNormalTextureID != INVALID_DX11_MATERIAL_ID;

				if (hasDiffuseTexture)
					mMaterialMap.GetItem(material.mDiffuseTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_DIFFUSE);

				if (hasNormalTexture)
					mMaterialMap.GetItem(material.mNormalTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_NORMAL);
			}

			const bool isAnimating = false;

			const Mat4 wvpMatrix = renderQueue.mCamera.mCameraViewProjectionMatrix * mesh.mWorldTransform;
			const Mat4 worldViewMatrix = renderQueue.mCamera.mCameraViewMatrix * mesh.mWorldTransform;

			mGBuffer.SetConstantData(wvpMatrix, worldViewMatrix, mesh.mMaterialTilingFactor, hasDiffuseTexture, hasNormalTexture, isAnimating);
			mMeshMap.GetItem(mesh.mMeshID).Draw();
		}


		// ANIMATED
		const auto animatedBeginIndex = renderQueue.mCamera.mAnimatedMeshesBegin;
		const auto animatedEndIndex = renderQueue.mCamera.mAnimatedMeshesEnd;
		for (auto meshIndex = animatedBeginIndex; meshIndex < animatedEndIndex; ++meshIndex)
		{
			const RenderableMesh& mesh = renderQueue.mRenderData.mAnimatedMeshes.at(meshIndex);
			assert(mesh.mMeshID != INVALID_DX11_MESH_ID);

			bool hasDiffuseTexture = false, hasNormalTexture = false;
			if (mesh.mMaterial != RenderableMaterial::INVALID_INDEX)
			{
				const RenderableMaterial& material = renderQueue.mRenderData.mMaterials.at(mesh.mMaterial);
				hasDiffuseTexture = material.mDiffuseTextureID != INVALID_DX11_MATERIAL_ID;
				hasNormalTexture = material.mNormalTextureID != INVALID_DX11_MATERIAL_ID;

				if (hasDiffuseTexture)
					mMaterialMap.GetItem(material.mDiffuseTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_DIFFUSE);

				if (hasNormalTexture)
					mMaterialMap.GetItem(material.mNormalTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_NORMAL);
			}

			const bool isAnimating = true;

			const Mat4 wvpMatrix = renderQueue.mCamera.mCameraViewProjectionMatrix * mesh.mWorldTransform;
			const Mat4 worldViewMatrix = renderQueue.mCamera.mCameraViewMatrix * mesh.mWorldTransform;

			mGBuffer.SetConstantData(wvpMatrix, worldViewMatrix, mesh.mMaterialTilingFactor, hasDiffuseTexture, hasNormalTexture, isAnimating);
			mMeshMap.GetItem(mesh.mMeshID).Draw();
		}*/
    }

    void DX11Pipeline::LightingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugExtra, const EngineSettings::ShadowFiltering shadowFiltering, const bool SSAOEnabled)
    {
        mLightAccbuffer.Clear();
        mLightAccbuffer.BindAsRenderTarget(mDSVReadOnly);
        mGBuffer.BindGeometryTextures();

        // expose depth buffer as SRV
		// TODO: move elsewhere?
        mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);
		mContext->CSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);

        // disable further depth writing
        mContext->OMSetDepthStencilState(mDepthStencilState, 0);

        const Mat4 invCameraProjMatrix = glm::inverse(renderQueue.mCamera.mCameraProjectionMatrix);

        // ambient light
        mAmbientPass.Render(invCameraProjMatrix, renderQueue.mAmbientLight, mWindowSize, SSAOEnabled);

        // additive blending for adding lighting
        mContext->OMSetBlendState(mAdditiveBlending, nullptr, 0xffffffff);

        // do all directional lights
        for (const RenderableDirectionalLight& directionalLight : renderQueue.mDirectionalLights)
            mDirectionalLightPass.Render(directionalLight, renderQueue.mRenderData, shadowFiltering, renderQueue.mCamera.mFOV, renderQueue.mCamera.mCameraViewMatrix, invCameraProjMatrix);

        // do all point lights
        mPointLightPass.BindForShading();
        for (const RenderablePointLight& pointLight : renderQueue.mPointLights)
        {
            mContext->ClearDepthStencilView(mDSV, D3D11_CLEAR_STENCIL, 1.0f, 0);
            mPointLightPass.Render(pointLight, renderQueue.mRenderData, renderQueue.mCamera.mCameraViewMatrix, renderQueue.mCamera.mCameraViewProjectionMatrix, invCameraProjMatrix);
        }

        // turn off blending
        mContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }

    void DX11Pipeline::PostProcessingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::AntiAliasing AA)
    {
        // flip from lightAccumulatorBuffer --> backbuffer
        mBackbuffer.BindForDrawing(mDSVReadOnly, true);
        mLightAccbuffer.BindAsShaderResource(SHADER_TEXTURE_SLOT_EXTRA);
        mBackbuffer.FillBackbuffer();

        // post-processing done in sRGB space
        mBackbuffer.BindForDrawing(mDSVReadOnly, false);

        // FXAA done in sRGB space
        if (AA == EngineSettings::AntiAliasing::FXAA)
            mPostProcessor.FXAAPass(mBackbuffer, mWindowSize);

        if (renderQueue.mSkyboxTextureID != INVALID_DX11_MATERIAL_ID)
            mSkyboxPass.Render(renderQueue.mCamera.mCameraViewMatrix, renderQueue.mCamera.mCameraProjectionMatrix, mMaterialMap.GetItem(renderQueue.mSkyboxTextureID));

        if (debugFlags.test(DebugOptions::RENDER_FLAG_DRAW_AABB))
            mAABBPass.Render(renderQueue.mRenderData, renderQueue.mCamera);
    }


	void DX11Pipeline::RenderMeshes(const RenderQueue& renderQueue, const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index begin, const RenderableMesh::Index end)
	{

	}
}