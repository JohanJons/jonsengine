#include "include/Renderer/DirectX11/DX11AABBPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleColorPixel.h"

namespace JonsEngine
{
    DX11AABBPass::DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass) :
        mContext(context), mPixelShader(nullptr), mVertexTransformPass(vertexTransformPass)
    {
        DXCALL(device->CreatePixelShader(gSimpleColorPixelShader, sizeof(gSimpleColorPixelShader), nullptr, &mPixelShader));
    }
    
    DX11AABBPass::~DX11AABBPass()
    {
    }


    void DX11AABBPass::Render(const RenderableAABBsContainer& renderData, const Mat4& viewProj )
    {
        mContext->PSSetShader(mPixelShader, nullptr, 0);

        mVertexTransformPass.RenderAABBs( renderData, viewProj );
    }
}