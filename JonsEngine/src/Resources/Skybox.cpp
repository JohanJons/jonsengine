#include "include/Resources/Skybox.h"

namespace JonsEngine
{
    Skybox::Skybox(const std::string& name, const DX11MaterialID skyboxTexture) : mName(name), mSkyboxTexture(skyboxTexture)
    {
    }

    Skybox::~Skybox()
    {
    }
}