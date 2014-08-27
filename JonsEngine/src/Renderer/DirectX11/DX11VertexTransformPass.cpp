#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformVertex.h"

namespace JonsEngine
{
    DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device) : mTransformCBuffer(device, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(&inputDescription, 1, gTransformVertexShader, sizeof(gTransformVertexShader), &mInputLayout));

        DXCALL(device->CreateVertexShader(gTransformVertexShader, sizeof(gTransformVertexShader), NULL, &mVertexShader));
    }

    DX11VertexTransformPass::~DX11VertexTransformPass()
    {
    }


    void DX11VertexTransformPass::BindForTransformPass(ID3D11DeviceContextPtr context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);

        context->VSSetShader(mVertexShader, NULL, NULL);
    }

    void DX11VertexTransformPass::RenderMesh(ID3D11DeviceContextPtr context, DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix), context);
        mesh.Draw(context);
    }

    void DX11VertexTransformPass::RenderMeshes(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const Mat4& viewProjectionMatrix)
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

            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * renderable.mWorldMatrix), context);
            (*meshIterator)->Draw(context);
        }
    }
}