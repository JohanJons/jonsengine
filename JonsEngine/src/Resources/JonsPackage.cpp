#include "include/Resources/JonsPackage.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <limits>

namespace JonsEngine
{
    PackageHeader::PackageHeader() : mSignature("jons"), mMajorVersion(LatestMajorVersion), mMinorVersion(LatestMinorVersion)
    {
    }

    PackageAABB::PackageAABB() : mMinBounds(std::numeric_limits<float>::max()), mMaxBounds(std::numeric_limits<float>::lowest())
    {
    }

    PackageMesh::PackageMesh() : mMaterialIndex(0), mHasMaterial(false)
    {
    }

    PackageNode::PackageNode() : mNodeID(INVALID_NODE_ID)
    {
    }

    PackageTexture::PackageTexture() : mTextureWidth(0), mTextureHeight(0)
    {
    }

    PackageSkybox::PackageSkybox()
    {
    }

    PackageSkybox::PackageSkybox(const std::string& name) : mName(name)
    {
    }

    PackageMaterial::PackageMaterial() : mHasDiffuseTexture(false), mHasNormalTexture(false), mDiffuseColor(1.0f), mAmbientColor(1.0f), mSpecularColor(1.0f), mEmissiveColor(0.0f)
    {
    }

    PackageMaterial::PackageMaterial(const std::string& name, const bool hasDiffTexture, const bool hasNormalTexture) : mName(name), mHasDiffuseTexture(hasDiffTexture), mHasNormalTexture(hasNormalTexture)
    {
    }

    PackageAnimatedNodeTransform::PackageAnimatedNodeTransform()
    {
    }

    PackageAnimatedNodeTransform::PackageAnimatedNodeTransform(const double timestamp, const Mat4& transform) : mTimestamp(timestamp), mTransform(transform)
    {
    }

    PackageAnimatedNode::PackageAnimatedNode()
    {
    }

    PackageAnimatedNode::PackageAnimatedNode(const std::string& nodeName) : mNodeName(nodeName)
    {
    }

    PackageAnimation::PackageAnimation()
    {
    }

    PackageAnimation::PackageAnimation(const std::string& name, const double durationSeconds) : mName(name), mDurationInSeconds(durationSeconds)
    {
    }

    PackageModel::PackageModel()
    {
    }

    PackageModel::PackageModel(const std::string& name) : mName(name)
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