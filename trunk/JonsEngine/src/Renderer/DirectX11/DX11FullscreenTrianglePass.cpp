#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"

namespace JonsEngine
{
    DX11FullscreenTrianglePass::DX11FullscreenTrianglePass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) :
        mContext(context), mVertexShader(nullptr)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
    }

    DX11FullscreenTrianglePass::~DX11FullscreenTrianglePass()
    {
    }


    void DX11FullscreenTrianglePass::Render()
    {
        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mContext->IASetInputLayout(NULL);

        mContext->VSSetShader(mVertexShader, NULL, NULL);

        mContext->Draw(3, 0);
    }
}