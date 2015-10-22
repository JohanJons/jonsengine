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

    PackageNode::PackageNode()
    {
    }

    PackageNode::PackageNode(const std::string& name, const uint32_t nodeIndex, const uint32_t parentNodeIndex) : mName(name), mNodeIndex(nodeIndex), mParentNodeIndex(parentNodeIndex)
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

    PackageAnimatedNodeTransform::PackageAnimatedNodeTransform(const uint32_t timestampMilliseconds, const Mat4& transform) : mTimestampMilliseconds(timestampMilliseconds), mTransform(transform)
    {
    }

    PackageAnimatedNode::PackageAnimatedNode()
    {
    }

    PackageAnimatedNode::PackageAnimatedNode(const uint32_t nodeIndex) : mNodeIndex(nodeIndex)
    {
    }

    PackageAnimation::PackageAnimation()
    {
    }

    PackageAnimation::PackageAnimation(const std::string& name, const uint32_t animationIndex, const uint32_t durationInMilliseconds) : mName(name), mAnimationIndex(animationIndex), mDurationInMilliseconds(durationInMilliseconds)
    {
    }

    PackageModel::PackageModel()
    {
    }

    PackageModel::PackageModel(const std::string& modelName) : mName(modelName)
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