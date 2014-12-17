#include "include/Renderer/DirectX11/DX11Pipeline.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Pipeline::DX11Pipeline(ID3D11DevicePtr device, D3D11_TEXTURE2D_DESC backbufferTextureDesc) : mBackbuffer(), mLightAccbuffer()
    {
    }

    DX11Pipeline::~DX11Pipeline()
    {
    }
}