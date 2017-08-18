#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/Core/Types.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	struct AABBRenderData
	{
		AABBRenderData();


		// TODO: for multiple cameras a better solution could be had
		Mat4 mCameraViewProjectionMatrix;
		// AABB rendering only requires worldMatrix, not localWorldMatrix (as local is already precomputed into mesh/model AABB)
		std::vector<Mat4> mAABBTransforms;
	};
}