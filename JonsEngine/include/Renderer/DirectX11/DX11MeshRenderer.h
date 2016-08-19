#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Core/Containers/IDMap.hpp"

namespace JonsEngine
{
    class DX11MeshRenderer
    {
    public:
        DX11MeshRenderer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDMap<DX11Mesh>& meshMap);
        
        void RenderAABBs();

        
    private:
    };
}