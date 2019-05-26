#pragma once

#include "Core/Types.h"
#include "Resources/Bone.h"
#include "Renderer/RenderDefs.h"

#include <limits>
#include <vector>

namespace JonsEngine
{
	typedef std::pair<Mat4, DX11MeshID> AABBRenderData;
	typedef std::pair<Color, std::vector<AABBRenderData>> AABBsOfColor;
	typedef std::vector<AABBsOfColor> RenderableAABBsContainer;

	template <typename RENDERABLE>
	struct RenderableContainer
	{
		typedef std::vector<RENDERABLE> ContainerType;
		typename typedef ContainerType::size_type Index;
		static constexpr Index INVALID_INDEX = std::numeric_limits<Index>::max();
	};

	struct RenderableSkeleton
	{
		RenderableSkeleton::RenderableSkeleton() : RenderableSkeleton( INVALID_BONE_INDEX, INVALID_BONE_INDEX )
		{ }
		RenderableSkeleton::RenderableSkeleton( const BoneIndex begin, const BoneIndex end ) :
			mBoneRange( begin, end )
		{ }
		RenderableSkeleton::RenderableSkeleton( const BoneIndexRange& boneRange ) :
			mBoneRange( boneRange )
		{ }

		BoneIndexRange mBoneRange;
	};

	struct RenderableMaterial : RenderableContainer<RenderableMaterial>
	{
		RenderableMaterial::RenderableMaterial( const DX11TextureID diffuseTexture, const DX11TextureID normalTexture, const float specFactor ) :
			mDiffuseTextureID( diffuseTexture ),
			mNormalTextureID( normalTexture ),
			mSpecularFactor( specFactor )
		{ }

		DX11TextureID mDiffuseTextureID;
		DX11TextureID mNormalTextureID;
		float mSpecularFactor;
	};

	typedef RenderableMaterial::Index MaterialIndex;

	struct RenderableMesh : RenderableContainer<RenderableMesh>
	{
		RenderableMesh::RenderableMesh( const Mat4& localWorldTransform, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID ) :
			mLocalWorldTransform( localWorldTransform ),
			mMaterial( materialIndex ),
			mMaterialTilingFactor( materialTilingFactor ),
			mMeshID( meshID )
		{ }
		RenderableMesh::RenderableMesh( const Mat4& localWorldTransform, const BoneIndexRange& boneRange, const MaterialIndex materialIndex, const float materialTilingFactor, const DX11MeshID meshID ) :
			mLocalWorldTransform( localWorldTransform ),
			mSkeleton( boneRange ),
			mMaterial( materialIndex ),
			mMaterialTilingFactor( materialTilingFactor ),
			mMeshID( meshID )
		{ }

		Mat4 mLocalWorldTransform;
		RenderableSkeleton mSkeleton;
		MaterialIndex mMaterial;
		float mMaterialTilingFactor;
		DX11MeshID mMeshID;
	};

	typedef RenderableMesh::Index MeshIndex;
	typedef RenderableMesh::ContainerType RenderableMeshContainer;

	struct RenderableCollection
	{
		RenderableCollection::RenderableCollection() :
			mStaticMeshesBegin( RenderableMesh::INVALID_INDEX ),
			mStaticMeshesEnd( RenderableMesh::INVALID_INDEX ),
			mAnimatedMeshesBegin( RenderableMesh::INVALID_INDEX ),
			mAnimatedMeshesEnd( RenderableMesh::INVALID_INDEX )
		{ }

		MeshIndex mStaticMeshesBegin;
		MeshIndex mStaticMeshesEnd;
		MeshIndex mAnimatedMeshesBegin;
		MeshIndex mAnimatedMeshesEnd;
	};

	struct RenderableDirectionalLight
	{
		struct CascadeSplit : RenderableCollection
		{
			RenderableDirectionalLight::CascadeSplit::CascadeSplit() :
				mNearZ( 0.0f ),
				mFarZ( 0.0f )
			{ }

			float mNearZ;
			float mFarZ;
		};

		RenderableDirectionalLight::RenderableDirectionalLight( const Vec4& color, const Vec3& direction, const uint32_t numCascades ) :
			mColor( color ),
			mDirection( direction ),
			mNumCascadesUsed( numCascades )
		{ }

		Vec4 mColor;
		Vec3 mDirection;
		uint32_t mNumCascadesUsed;
		std::array<CascadeSplit, gMaxNumShadowmapCascades> mCascades;
	};

	struct RenderablePointLight : RenderableCollection
	{
		RenderablePointLight::RenderablePointLight( const Vec4& color, const Vec3& position, const float intensity, const float radius ) :
			mColor( color ),
			mPosition( position ),
			mIntensity( intensity ),
			mRadius( radius )
		{ }

		Vec4 mColor;
		Vec3 mPosition;
		float mIntensity;
		float mRadius;
	};

	typedef std::vector<RenderablePointLight> RenderablePointLights;
	typedef std::vector<RenderableDirectionalLight> RenderableDirectionalLights;

	struct RenderData
	{
		RenderableMeshContainer mStaticMeshes;
		RenderableMeshContainer mAnimatedMeshes;
		RenderableMaterial::ContainerType mMaterials;
		BoneTransforms mBones;
	};

	struct RenderableCamera : RenderableCollection
	{
		Mat4 mCameraViewMatrix;
		Mat4 mCameraProjectionMatrix;
		Mat4 mCameraViewProjectionMatrix;
		Vec3 mCameraPosition;
		float mFOV;
	};

	struct RenderableTerrainData
	{
		RenderableTerrainData( DX11TextureID heightMap, uint32_t endIndex, Vec2 worldMin, Vec2 worldMax, float heightScale, float variationScale ) :
			mHeightMap( heightMap ),
			mEndIndex( endIndex ),
			mWorldMin( worldMin ),
			mWorldMax( worldMax ),
			mHeightScale( heightScale ),
			mVariationScale( variationScale )
		{ }

		DX11TextureID mHeightMap;
		uint32_t mEndIndex;
		Vec2 mWorldMin;
		Vec2 mWorldMax;
		float mHeightScale;
		float mVariationScale;
	};

	struct RenderableTerrains
	{
		std::vector<RenderableTerrainData> mTerrainData;
		std::vector<Mat4> mTransforms;

		uint32_t GetNumTerrains() const { return static_cast<uint32_t>( mTerrainData.size() ); }
	};
}