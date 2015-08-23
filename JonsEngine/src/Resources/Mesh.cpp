#include "include/Resources/Mesh.h"

namespace JonsEngine
{
    Mesh::Mesh(const std::string& name, const DX11MeshID meshID, const MaterialID defaultMaterialID, const Vec3& minBounds, const Vec3& maxBounds) :
        mName(name), mMeshID(meshID), mLocalAABB(minBounds, maxBounds), mDefaultMaterialID(defaultMaterialID)
    {
    }

    Mesh::~Mesh()
    {
    }
}