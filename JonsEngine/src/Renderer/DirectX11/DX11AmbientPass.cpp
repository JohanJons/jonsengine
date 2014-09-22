#include "include/Renderer/DirectX11/DX11AmbientPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass) : 
        mFullscreenPass(fullscreenPass), mPixelShader(nullptr), mConstantBuffer(device, mConstantBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        DXCALL(device->CreatePixelShader(gAmbientPixelShader, sizeof(gAmbientPixelShader), NULL, &mPixelShader));
    }

    DX11AmbientPass::~DX11AmbientPass()
    {
    }


    void DX11AmbientPass::Render(ID3D11DeviceContextPtr context, const Vec4& ambientLight)
    {
        context->PSSetShader(mPixelShader, NULL, NULL);
        mConstantBuffer.SetData(AmbientCBuffer(ambientLight), context);

        mFullscreenPass.Render(context);
    }
}
