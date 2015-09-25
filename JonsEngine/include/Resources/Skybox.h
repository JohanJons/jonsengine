#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const DX11MaterialID skyboxTexture);
        ~Skybox();

        const std::string& GetName() const;
        DX11MaterialID GetSkyboxTexture() const;


    private:
         std::string mName;
         DX11MaterialID mSkyboxTexture;
    };

    typedef IDMap<Skybox>::ItemID SkyboxID;
    static const SkyboxID INVALID_SKYBOX_ID = IDMap<Skybox>::INVALID_ITEM_ID;
}