#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
    EngineSettings::EngineSettings() :  WindowTitle("JonsEngine Game"),
                                        RenderBackend(RenderBackend::OPENGL),
                                        InputBackend(InputBackend::GLFW)
    {
    }
}