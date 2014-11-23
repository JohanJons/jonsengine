#include "include/Renderer/DirectX11/DX11AABBPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleColorPixel.h"

namespace JonsEngine
{
    DX11AABBPass::DX11AABBPass(ID3D11DevicePtr device, DX11VertexTransformPass& vertexTransformPass) : mVertexTransformPass(vertexTransformPass), mPixelShader(nullptr)
    {
        DXCALL(device->CreatePixelShader(gSimpleColorPixelShader, sizeof(gSimpleColorPixelShader), NULL, &mPixelShader));
    }
    
    DX11AABBPass::~DX11AABBPass()
    {
    }


    void DX11AABBPass::Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const Mat4& viewProjectionMatrix)
    {
        context->PSSetShader(mPixelShader, NULL, NULL);
        mVertexTransformPass.BindForTransformPass(context, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

        mVertexTransformPass.RenderAABBs(context, renderQueue, meshes, viewProjectionMatrix);
    }
}
