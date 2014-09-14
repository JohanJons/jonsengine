#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"

namespace JonsEngine
{
    DX11FullscreenTrianglePass::DX11FullscreenTrianglePass(ID3D11DevicePtr device)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
    }

    DX11FullscreenTrianglePass::~DX11FullscreenTrianglePass()
    {
    }


    void DX11FullscreenTrianglePass::Render(ID3D11DeviceContextPtr context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        context->IASetInputLayout(NULL);

        context->VSSetShader(mVertexShader, NULL, NULL);

        context->Draw(3, 0);
    }
}