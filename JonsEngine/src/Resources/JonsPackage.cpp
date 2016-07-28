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
        mTextureWidth(0),
        mTextureHeight(0)
    {
    }

    PackageSkybox::PackageSkybox() :
        PackageSkybox("")
    {
    }

    PackageSkybox::PackageSkybox(const std::string& name) :
        mName(name)
    {
    }

    PackageMaterial::PackageMaterial() :
        PackageMaterial("", false, false)
    {
    }

    PackageMaterial::PackageMaterial(const std::string& name, const bool hasDiffTexture, const bool hasNormalTexture) : mName(name), mHasDiffuseTexture(hasDiffTexture), mHasNormalTexture(hasNormalTexture),
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

    PackageMesh::PackageMesh() : PackageMesh("", PackageBone::INVALID_BONE_INDEX, PackageBone::INVALID_BONE_INDEX)
    {
    }

    PackageMesh::PackageMesh(const std::string& name, PackageBone::BoneIndex boneStartIndex, PackageBone::BoneIndex boneEndIndex) :
        mName(name),
        mStartBoneIndex(boneStartIndex),
        mEndBoneIndex(boneEndIndex),
        mMaterialIndex(PackageMaterial::INVALID_MATERIAL_INDEX)
    {
    }

    PackageNode::PackageNode() :
        PackageNode("", INVALID_NODE_INDEX, INVALID_NODE_INDEX)
    {
    }

    PackageNode::PackageNode(const std::string& name, const NodeIndex nodeIndex, const NodeIndex parentNodeIndex) :
        mName(name),
        mNodeIndex(nodeIndex),
        mParentNodeIndex(parentNodeIndex)
    {
    }

    PackageAnimation::PackageAnimation()
    {
    }

    PackageAnimation::PackageAnimation(const std::string& name, const Mat4& invRootMatrix, const uint32_t durationInMilliseconds) :
        mName(name),
        mInverseRootMatrix(mInverseRootMatrix),
        mDurationInMilliseconds(durationInMilliseconds)
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