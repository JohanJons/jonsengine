#include "include/RenderQueue/RenderableCollection.h"

namespace JonsEngine
{
    RenderableCollection::RenderableCollection() :
        mStaticMeshesBegin(RenderableMesh::INVALID_INDEX),
        mStaticMeshesEnd(RenderableMesh::INVALID_INDEX),
        mAnimatedMeshesBegin(RenderableMesh::INVALID_INDEX),
        mAnimatedMeshesEnd(RenderableMesh::INVALID_INDEX)
    {
    }
}