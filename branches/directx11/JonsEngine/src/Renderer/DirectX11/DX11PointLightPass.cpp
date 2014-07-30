#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"

namespace JonsEngine
{
    DX11PointLightPass::DX11PointLightPass(ID3D11Device* device) : mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
        mVertexShader->Release();
        mPixelShader->Release();
    }
}