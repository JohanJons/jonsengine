#include "include/Resources/JonsPackage.h"
#include "include/Core/Memory/HeapAllocator.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>

namespace JonsEngine
{
    uint32_t gCurrentJonsPackageID = 0;
    std::map<uint32_t, std::string> gJonsPackageIDFileMap;


    PackageHeader::PackageHeader() : mSignature("jons"), mMajorVersion(LatestMajorVersion), mMinorVersion(LatestMinorVersion)
    {
    }

    PackageModel::PackageModel() : mName(""), mTransform(1.0f)
    {
    }

    PackageMesh::PackageMesh() : mMaterialIndex(0), mHasMaterial(false)
    {
    }

    PackageTexture::PackageTexture() : mName(""), mTextureWidth(0), mTextureHeight(0), mTextureType(TextureType::TEXTURE_TYPE_DIFFUSE)
    {
    }

    PackageMaterial::PackageMaterial() : mName(""), mHasDiffuseTexture(false), mHasNormalTexture(false), mDiffuseColor(1.0f), mAmbientColor(1.0f), mSpecularColor(1.0f), mEmissiveColor(0.0f)
    {
    }

    JonsPackage::JonsPackage() : mInternalID(gCurrentJonsPackageID++)
    {
    }


    JonsPackagePtr ReadJonsPkg(const std::string& jonsPkgName)
    {
        std::ifstream jonsPkgStream(jonsPkgName.c_str(), std::ios::in | std::ios::binary);        // TODO: support opening of older resource packages
        JonsPackagePtr pkg(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<JonsPackage>(), std::bind(&HeapAllocator::DeallocateObject<JonsPackage>, &HeapAllocator::GetDefaultHeapAllocator(), std::placeholders::_1));

        gJonsPackageIDFileMap.insert(std::make_pair(pkg->mInternalID, jonsPkgName));

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

    std::string GetJonsPkgPath(const uint32_t internalPackageID)
    {
        auto iter = std::find_if(gJonsPackageIDFileMap.begin(), gJonsPackageIDFileMap.end(), [internalPackageID](std::pair<uint32_t, std::string> entry) { return entry.first == internalPackageID; });

        if (iter != gJonsPackageIDFileMap.end())
            return iter->second;
        else
            return std::string();
    }
}