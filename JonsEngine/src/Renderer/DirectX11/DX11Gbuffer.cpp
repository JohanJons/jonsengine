#include "include/Renderer/DirectX11/DX11GBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferVertexStatic.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferVertexAnimated.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferPixel.h"

namespace JonsEngine
{
    enum VSInputLayout
    {
        POSITION = 0,
        NORMAL,
        TEXCOORD,
        TANGENT,
        BITANGENT,
        BONE_INDEX,
        BONE_WEIGHT,
        NUM_INPUT_LAYOUTS
    };

    DX11GBuffer::DX11GBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mContext(context),

		mStaticLayout(nullptr),
		mAnimatedLayout(nullptr),
		mStaticVertexShader(nullptr),
		mAnimatedVertexShader(nullptr),

		mPixelShader(nullptr),
		mConstantBuffer(device, context, mConstantBuffer.CONSTANT_BUFFER_SLOT_VERTEX)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        
        // color texture
        uint32_t gbufferIndex = DX11GBuffer::GBUFFER_RENDERTARGET_INDEX_DIFFUSE;
        backbufferTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, nullptr, &mTextures.at(gbufferIndex)));
        DXCALL(device->CreateRenderTargetView(mTextures.at(gbufferIndex), nullptr, &mRenderTargets.at(gbufferIndex)));
        DXCALL(device->CreateShaderResourceView(mTextures.at(gbufferIndex), nullptr, &mShaderResourceViews.at(gbufferIndex)));

        // normal texture
        gbufferIndex = DX11GBuffer::GBUFFER_RENDERTARGET_INDEX_NORMAL;
        backbufferTextureDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, nullptr, &mTextures.at(gbufferIndex)));
        DXCALL(device->CreateRenderTargetView(mTextures.at(gbufferIndex), nullptr, &mRenderTargets.at(gbufferIndex)));
        DXCALL(device->CreateShaderResourceView(mTextures.at(gbufferIndex), nullptr, &mShaderResourceViews.at(gbufferIndex)));

        // input layout
        D3D11_INPUT_ELEMENT_DESC inputDescription[NUM_INPUT_LAYOUTS];
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * NUM_INPUT_LAYOUTS);

        inputDescription[VSInputLayout::POSITION].SemanticName = "POSITION";
        inputDescription[VSInputLayout::POSITION].SemanticIndex = 0;
        inputDescription[VSInputLayout::POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[VSInputLayout::POSITION].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_POSITIONS;
        inputDescription[VSInputLayout::POSITION].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::POSITION].InstanceDataStepRate = 0;

        inputDescription[VSInputLayout::NORMAL].SemanticName = "NORMAL";
        inputDescription[VSInputLayout::NORMAL].SemanticIndex = 0;
        inputDescription[VSInputLayout::NORMAL].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[VSInputLayout::NORMAL].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS;
        inputDescription[VSInputLayout::NORMAL].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::NORMAL].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::NORMAL].InstanceDataStepRate = 0;

        inputDescription[VSInputLayout::TEXCOORD].SemanticName = "TEXCOORD";
        inputDescription[VSInputLayout::TEXCOORD].SemanticIndex = 0;
        inputDescription[VSInputLayout::TEXCOORD].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputDescription[VSInputLayout::TEXCOORD].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS;
        inputDescription[VSInputLayout::TEXCOORD].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::TEXCOORD].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::TEXCOORD].InstanceDataStepRate = 0;

        // tangents/bitangents share buffer slot
        inputDescription[VSInputLayout::TANGENT].SemanticName = "TANGENT";
        inputDescription[VSInputLayout::TANGENT].SemanticIndex = 0;
        inputDescription[VSInputLayout::TANGENT].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[VSInputLayout::TANGENT].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS;
        inputDescription[VSInputLayout::TANGENT].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::TANGENT].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::TANGENT].InstanceDataStepRate = 0;
        inputDescription[VSInputLayout::BITANGENT].SemanticName = "BITANGENT";
        inputDescription[VSInputLayout::BITANGENT].SemanticIndex = 0;
        inputDescription[VSInputLayout::BITANGENT].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[VSInputLayout::BITANGENT].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS;
        inputDescription[VSInputLayout::BITANGENT].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        inputDescription[VSInputLayout::BITANGENT].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::BITANGENT].InstanceDataStepRate = 0;

        inputDescription[VSInputLayout::BONE_INDEX].SemanticName = "BONE_INDICES";
        inputDescription[VSInputLayout::BONE_INDEX].SemanticIndex = 0;
        inputDescription[VSInputLayout::BONE_INDEX].Format = DXGI_FORMAT_R8G8B8A8_UINT;
        inputDescription[VSInputLayout::BONE_INDEX].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
        inputDescription[VSInputLayout::BONE_INDEX].AlignedByteOffset = 0;
        inputDescription[VSInputLayout::BONE_INDEX].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::BONE_INDEX].InstanceDataStepRate = 0;
        inputDescription[VSInputLayout::BONE_WEIGHT].SemanticName = "BONE_WEIGHTS";
        inputDescription[VSInputLayout::BONE_WEIGHT].SemanticIndex = 0;
        //inputDescription[VSInputLayout::BONE_WEIGHT].Format = DXGI_FORMAT_R16G16B16A16_UNORM;
        // NOTE: TEMP! use smaller layout...
        inputDescription[VSInputLayout::BONE_WEIGHT].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        inputDescription[VSInputLayout::BONE_WEIGHT].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_BONE_WEIGHTS;
        inputDescription[VSInputLayout::BONE_WEIGHT].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        inputDescription[VSInputLayout::BONE_WEIGHT].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[VSInputLayout::BONE_WEIGHT].InstanceDataStepRate = 0;

		const uint32_t animatedNumExtraInputs = 2;
        DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS - animatedNumExtraInputs, gGBufferVertexStaticShader, sizeof(gGBufferVertexStaticShader), &mStaticLayout));
		DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gGBufferVertexAnimatedShader, sizeof(gGBufferVertexAnimatedShader), &mAnimatedLayout));

        // create shader objects
        DXCALL(device->CreateVertexShader(gGBufferVertexStaticShader, sizeof(gGBufferVertexStaticShader), nullptr, &mStaticVertexShader));
		DXCALL(device->CreateVertexShader(gGBufferVertexAnimatedShader, sizeof(gGBufferVertexAnimatedShader), nullptr, &mAnimatedVertexShader));
        DXCALL(device->CreatePixelShader(gGBufferPixelShader, sizeof(gGBufferPixelShader), nullptr, &mPixelShader));
    }

    DX11GBuffer::~DX11GBuffer()
    {
    }


    void DX11GBuffer::SetConstantData(const Mat4& wvpMatrix, const Mat4& worldViewMatrix, const float textureTilingFactor, const bool hasDiffuseTexture, const bool hasNormalTexture, const uint32_t boneIndexOffset)
    {
        mConstantBuffer.SetData({ wvpMatrix, worldViewMatrix, textureTilingFactor, hasDiffuseTexture, hasNormalTexture, boneIndexOffset });
    }

    void DX11GBuffer::BindForGeometryStage(ID3D11DepthStencilViewPtr dsv)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; ++index)
        {
            // unbind gbuffer textures as input, it is now rendertarget
            mContext->PSSetShaderResources(index, 1, &gNullSRV.p);
            mContext->ClearRenderTargetView(mRenderTargets.at(index), &gClearColor.front());
        }
        
        // backbuffers depth texture might still be bound as SRV
        mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &gNullSRV.p);
		mContext->CSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &gNullSRV.p);
        mContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // default == depth testing/writing on
        mContext->OMSetDepthStencilState(nullptr, 0);
        mContext->OMSetRenderTargets(DX11GBuffer::GBUFFER_NUM_RENDERTARGETS, &(mRenderTargets.begin()->p), dsv);
        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mContext->PSSetShader(mPixelShader, nullptr, 0);
    }

	void DX11GBuffer::BindForStaticPass()
	{
		mContext->IASetInputLayout(mStaticLayout);
		mContext->VSSetShader(mStaticVertexShader, nullptr, 0);
	}

	void DX11GBuffer::BindForAnimatedPass()
	{
		mContext->IASetInputLayout(mAnimatedLayout);
		mContext->VSSetShader(mAnimatedVertexShader, nullptr, 0);
	}

    void DX11GBuffer::BindGeometryTextures()
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; ++index)
            mContext->PSSetShaderResources(index, 1, &mShaderResourceViews.at(index).p);
    }
}
