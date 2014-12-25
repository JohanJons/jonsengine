#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11LightAccumulationbuffer
    {
    public:
        DX11LightAccumulationbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11LightAccumulationbuffer();

        void BindForDrawing(ID3D11DepthStencilViewPtr dsv);
        void ClearAccumulationBuffer();

        ID3D11ShaderResourceViewPtr GetLightAccumulationBuffer();


    private:
        ID3D11DeviceContextPtr mContext;

        ID3D11Texture2DPtr mAccumulationTexture;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11ShaderResourceViewPtr mSRV;
    };
}
