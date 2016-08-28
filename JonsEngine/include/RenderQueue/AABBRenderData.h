#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/Core/Types.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	struct AABBRenderData
	{
		// AABB rendering only requires worldMatrix, not localWorldMatrix (as local is already precomputed into mesh/model AABB)
		typedef std::pair<Mat4, DX11MeshID> RenderAABBPair;
		typedef std::vector<RenderAABBPair> RenderableAABBsContainer;

		AABBRenderData();


		// TODO: for multiple cameras a better solution could be had
		Mat4 mCameraViewProjectionMatrix;
		RenderableAABBsContainer mRenderableAABBs;
	};
}