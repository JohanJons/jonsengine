#pragma once

#include "include/RenderQueue/Renderable.h"
#include "include/RenderQueue/RenderableCamera.h"
#include "include/RenderQueue/RenderableDirectionalLigth.h"
#include "include/RenderQueue/RenderablePointLight.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/RenderQueue/RenderableMesh.h"
#include "include/RenderQueue/RenderableSkeleton.h"

namespace JonsEngine
{
    struct RenderQueue
    {

        RenderableBones mBones;
    };
}