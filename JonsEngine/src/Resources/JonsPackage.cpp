#include "include/Resources/JonsPackage.h"

#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Math/Math.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <limits>

namespace JonsEngine
{
    PackageHeader::PackageHeader() :
        mSignature("jons"),
        mMajorVersion(LatestMajorVersion),
        mMinorVersion(LatestMinorVersion)
    {
    }

    PackageAABB::PackageAABB() :
        mMinBounds(std::numeric_limits<float>::max()),
        mMaxBounds(std::numeric_limits<float>::lowest())
    {
    }

	PackageTexture::PackageTexture() :
		PackageTexture("UNKNOWN", TextureType::Diffuse)
	{
	}

    PackageTexture::PackageTexture(const std::string& name, TextureType type) :
		mName(name),
		mType(type),
        mTextureWidth(0),
        mTextureHeight(0)
    {
    }

	PackageTerrainMap::PackageTerrainMap() :
		PackageTerrainMap("UNKNOWN", PackageTexture::INVALID_TEXTURE_INDEX, std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest())
	{
	}

	PackageTerrainMap::PackageTerrainMap(const std::string& name, PackageTexture::TextureIndex heightMapTexture, float minElevation, float maxElevation) :
		mName(name),
		mTerrainTexture(heightMapTexture),
		mMinElevation(minElevation),
		mMaxElevation(maxElevation)
	{
	}

    PackageMaterial::PackageMaterial() :
        PackageMaterial("")
    {
    }

    PackageMaterial::PackageMaterial(const std::string& name) :
		mName(name),
		mDiffuseTexture(PackageTexture::INVALID_TEXTURE_INDEX),
		mNormalTexture(PackageTexture::INVALID_TEXTURE_INDEX),
        mDiffuseColor(1.0f),
        mAmbientColor(1.0f),
        mSpecularColor(1.0f),
        mEmissiveColor(0.0f)
    {
    }

    PackageBone::PackageBone() :
        mName(""),
        mOffsetMatrix(gIdentityMatrix)
    {
    }

    PackageBone::PackageBone(const std::string& name, const Mat4& transform) :
        mName(name),
        mOffsetMatrix(transform)
    {
    }

	PackageBoneKeyframe::PackageBoneKeyframe() :
		mTimestampMillisec(0),
		mTranslation(0.0f),
		mRotation(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	PackageBoneKeyframe::PackageBoneKeyframe(const uint32_t timestampMillisec, const Vec3& translation, const Quaternion& rotation) :
		mTimestampMillisec(timestampMillisec),
		mTranslation(translation),
		mRotation(rotation)
	{
	}

    PackageMesh::PackageMesh() : PackageMesh("")
    {
    }

    PackageMesh::PackageMesh(const std::string& name) :
        mName(name),
        mMaterialIndex(PackageMaterial::INVALID_MATERIAL_INDEX)
    {
    }


    PackageNode::PackageNode() :
        PackageNode("", gIdentityMatrix, INVALID_NODE_INDEX, INVALID_NODE_INDEX)
    {
    }

	PackageNode::PackageNode(const std::string& name, const Mat4& transform, const NodeIndex nodeIndex, const NodeIndex parentNodeIndex) :
		mName(name),
		mTransform(transform),
		mNodeIndex(nodeIndex),
		mParentNodeIndex(parentNodeIndex)
    {
    }


    PackageAnimation::PackageAnimation()
    {
    }

    PackageAnimation::PackageAnimation(const std::string& name, const uint32_t durationInMilliseconds, const Mat4& invRootMatrix) :
        mName(name),
		mDurationInMilliseconds(durationInMilliseconds),
        mInverseRootMatrix(invRootMatrix)
    {
    }


    PackageModel::PackageModel()
    {
    }

    PackageModel::PackageModel(const std::string& modelName) :
        mName(modelName)
    {
    }


    JonsPackagePtr ReadJonsPkg(const std::string& jonsPkgName)
    {
        std::ifstream jonsPkgStream(jonsPkgName.c_str(), std::ios::in | std::ios::binary);        // TODO: support opening of older resource packages
        JonsPackagePtr pkg(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<JonsPackage>(), std::bind(&HeapAllocator::DeallocateObject<JonsPackage>, &HeapAllocator::GetDefaultHeapAllocator(), std::placeholders::_1));

        if (jonsPkgStream && jonsPkgStream.good() && jonsPkgStream.is_open())
        {
            boost::archive::binary_iarchive iar(jonsPkgStream);

            iar >> (*pkg.get());
        }
  
        return pkg;
    }

    bool WriteJonsPkg(const std::string& jonsPkgName, const JonsPackagePtr pkg)
    {
        std::ofstream outStream(jonsPkgName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
        bool ret = false;

        if (outStream.is_open())
        {
            boost::archive::binary_oarchive oar(outStream);
            oar << (*pkg.get());

            ret = true;
        }
        
        return ret;
    }
}