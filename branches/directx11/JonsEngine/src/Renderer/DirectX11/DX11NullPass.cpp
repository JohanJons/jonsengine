#include "include/Renderer/DirectX11/DX11NullPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/NullVertex.h"

namespace JonsEngine
{
    DX11NullPass::DX11NullPass(ID3D11DevicePtr device) : mNullCBuffer(device)
    {
        DXCALL(device->CreateVertexShader(gNullVertexShader, sizeof(gNullVertexShader), NULL, &mNullVertexShader));
    }

    DX11NullPass::~DX11NullPass()
    {

    }


    void DX11NullPass::BindForDepthStencilPass(ID3D11DeviceContextPtr context)
    {
        context->VSSetShader(mNullVertexShader, NULL, NULL);
        context->PSSetShader(NULL, NULL, NULL);
    }

    void DX11NullPass::RenderMesh(ID3D11DeviceContextPtr context, DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
        mNullCBuffer.SetData(NullCBuffer(wvpMatrix), context, 0);
        mesh.Draw(context);
    }

    void DX11NullPass::RenderMeshes(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const Mat4& viewProjectionMatrix)
    {
        auto meshIterator = meshes.begin();
        for (const Renderable& renderable : renderQueue)
        {
            if (renderable.mMesh == INVALID_MESH_ID)
            {
                JONS_LOG_ERROR(Logger::GetRendererLogger(), "Renderable MeshID is invalid");
                throw std::runtime_error("Renderable MeshID is invalid");
            }

            if (renderable.mMesh < (*meshIterator)->GetMeshID())
                continue;

            while (renderable.mMesh >(*meshIterator)->GetMeshID())
            {
                meshIterator++;
                if (meshIterator == meshes.end())
                {
                    JONS_LOG_ERROR(Logger::GetRendererLogger(), "Renderable MeshID out of range");
                    throw std::runtime_error("Renderable MeshID out of range");
                }
            }

            mNullCBuffer.SetData(NullCBuffer(viewProjectionMatrix * renderable.mWorldMatrix), context, 0);
            (*meshIterator)->Draw(context);
        }
    }
}