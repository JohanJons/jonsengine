#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/Core/Types.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	class RenderableAABBsContainer
	{
	public:
		typedef std::pair<Mat4, DX11MeshID> AABBRenderData;
		typedef std::pair<Color, std::vector<AABBRenderData>> AABBsOfColor;
			
		RenderableAABBsContainer() = default;
			
		void AddAABB( const Mat4& transform, DX11MeshID mesh, Color color )
		{
			auto iter = std::find_if( mColorsToAABBsList.begin(), mColorsToAABBsList.end(), [ &color ]( const AABBsOfColor& ColorAABBMap ) { return ColorAABBMap.first == color; } );
			if ( iter == mColorsToAABBsList.end() )
			{
				mColorsToAABBsList.emplace_back( color );
				AABBsOfColor& colorAABBs = mColorsToAABBsList.back();
				colorAABBs.second.emplace_back( transform, mesh );
			}
			else
			{
				iter->second.emplace_back( transform, mesh );
			}
		}
			
		const std::vector<AABBsOfColor>& GetColorToAABBs() const { return mColorsToAABBsList; }
			
		private:
			std::vector<AABBsOfColor> mColorsToAABBsList;
	};
}
