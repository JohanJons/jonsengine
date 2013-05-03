#include "include/Resources/JonsPackage.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include <string>
#include <iostream>
#include <fstream>

namespace JonsEngine
{
    JonsPackagePtr ReadJonsPkg(const std::string& jonsPkgName)
    {
        std::ifstream jonsPkgStream(jonsPkgName.c_str(), std::ios::in | std::ios::binary);        // TODO: support opening of older resource packages
        JonsPackagePtr pkg(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<JonsPackage>(), boost::bind(&HeapAllocator::DeallocateObject<JonsPackage>, &HeapAllocator::GetDefaultHeapAllocator(), _1));

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