#include "include/Renderer/DirectX11/DX11AABBPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleColorPixel.h"

namespace JonsEngine
{
	/*
        vertexContainer.push_back(maxBounds.x); vertexContainer.push_back(maxBounds.y); vertexContainer.push_back(maxBounds.z);
	*/

	const std::vector<float> gAABBMesh{
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 
		-1.0f, 1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
	};

	const std::vector<uint16_t> gAABBIndices{ 0, 1, 2, 3, 0, 5, 4, 3, 2, 7, 4, 7, 6, 5, 6, 1 };


    DX11AABBPass::DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass) :
        mContext(context),
		mPixelShader(nullptr),
		mAABBMesh(device, context, gAABBMesh, gAABBIndices),
		mVertexTransformPass(vertexTransformPass)
    {
        DXCALL(device->CreatePixelShader(gSimpleColorPixelShader, sizeof(gSimpleColorPixelShader), nullptr, &mPixelShader));
    }
    
    DX11AABBPass::~DX11AABBPass()
    {
    }


    void DX11AABBPass::Render(const AABBRenderData& renderData)
    {
        mContext->PSSetShader(mPixelShader, nullptr, 0);
        mVertexTransformPass.RenderStaticMeshes(renderData);
    }
}