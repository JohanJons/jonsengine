#pragma once

#include "include/Core/Types.h"

#include <string>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const DX11MaterialID skyboxTexture);
        ~Skybox();


        const std::string mName;
        const DX11MaterialID mSkyboxTexture;
    };
}