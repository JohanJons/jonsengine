#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11LightAccumulationbuffer
    {
    public:
        DX11LightAccumulationbuffer(ID3D11DevicePtr device, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11LightAccumulationbuffer();

        void BindForReading(ID3D11DeviceContextPtr context);
        void BindForLightingStage(ID3D11DeviceContextPtr context);

        void ClearAccumulationBuffer(ID3D11DeviceContextPtr context);
        void ClearStencilBuffer(ID3D11DeviceContextPtr context);

        ID3D11DepthStencilViewPtr GetDepthStencilView();


    private:
        ID3D11Texture2DPtr mAccumulationTexture;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11Texture2DPtr mDepthStencilBuffer;
        ID3D11DepthStencilViewPtr mDSV;
        ID3D11DepthStencilViewPtr mDSVReadOnly;
        ID3D11ShaderResourceViewPtr mDepthSRV;
        ID3D11DepthStencilStatePtr mDepthStencilState;
    };
}
