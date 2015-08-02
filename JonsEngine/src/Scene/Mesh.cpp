#include "include/Scene/Mesh.h"

namespace JonsEngine
{
    Mesh::Mesh(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID, MaterialPtr material) :
        mName(name), mMaterialTilingFactor(1.0f), mLocalAABB(minBounds, maxBounds), mMeshID(meshID), mMaterial(material)
    {
    }

    Mesh::~Mesh()
    {
    }


	float Mesh::GetTextureTilingFactor() const
	{
		return mMaterialTilingFactor;
	}

    void Mesh::SetTextureTilingFactor(const float tilingFactor)
	{
		mMaterialTilingFactor = tilingFactor;
	}


	MaterialPtr Mesh::GetMaterial() const
	{
		return mMaterial;
	}

	void Mesh::SetMaterial(MaterialPtr material)
	{
		mMaterial = material;
	}
}