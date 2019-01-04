#include "include/Renderer/DirectX11/DX11Texture.h"

namespace JonsEngine
{
    static DX11TextureID gNextTextureID = 1;

	DXGI_FORMAT GetTextureFormat( const TextureType textureType );
    uint32_t GetNumMipLevels(uint32_t width, uint32_t height);
	uint32_t GetBytesPerTextureFormat( DXGI_FORMAT textureFormat );


    DX11Texture::DX11Texture( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<uint8_t>& textureData, const TextureType textureType, const uint32_t textureWidth, const uint32_t textureHeight ) :
        mID( gNextTextureID++ ),
        mContext( context )
    {
		const bool isCubeTexture = textureType == TextureType::Skybox;
		const DXGI_FORMAT format = GetTextureFormat( textureType );

        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = isCubeTexture ? 6 : 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.MipLevels = 0;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if ( isCubeTexture )
            textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = isCubeTexture ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
        if ( isCubeTexture )
            srvDesc.TextureCube.MipLevels = -1;
        else
            srvDesc.Texture2D.MipLevels = -1;
        DXCALL(device->CreateShaderResourceView(mTexture, &srvDesc, &mSRV));

		// mip-level 0 data
		uint32_t sizeWidth = textureWidth * sizeof( uint8_t ) * GetBytesPerTextureFormat( format );
		if ( isCubeTexture )
		{
			for ( uint32_t index = 0; index < 6; ++index )
			{
				const uint32_t subResourceID = D3D11CalcSubresource( 0, index, GetNumMipLevels( textureWidth, textureHeight ) );
				context->UpdateSubresource( mTexture, subResourceID, NULL, &textureData.at( sizeWidth * textureHeight * index ), sizeWidth, 0 );
			}
		}
		else
			context->UpdateSubresource( mTexture, 0, NULL, &textureData.at( 0 ), sizeWidth, 0 );

		context->GenerateMips( mSRV );
    }


    void DX11Texture::BindAsShaderResource(const SHADER_TEXTURE_SLOT shaderTextureSlot) const
    {
		mContext->VSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
		mContext->DSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
        mContext->PSSetShaderResources( shaderTextureSlot, 1, &mSRV.p );
    }


	DXGI_FORMAT GetTextureFormat( const TextureType textureType )
	{
		switch ( textureType )
		{
			case TextureType::Diffuse:
			case TextureType::Skybox:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case TextureType::Height:
				return DXGI_FORMAT_R8_UNORM;
			case TextureType::Normal:
				return DXGI_FORMAT_R8G8B8A8_UNORM;

			default:
			{
				JONS_LOG_ERROR( Logger::GetRendererLogger(), "Bad TextureType provided" );
				throw std::runtime_error( "Bad TextureType provided" );
			}
		}
	}

    uint32_t GetNumMipLevels(uint32_t width, uint32_t height)
    {
        uint32_t numLevels = 1;
        const uint32_t minLevel = 1;
        while ((width > 1) || (height > 1))
        {
            width = std::max(width / 2, minLevel);
            height = std::max(height / 2, minLevel);
            ++numLevels;
        }

        return numLevels;
    }

	uint32_t GetBytesPerTextureFormat( DXGI_FORMAT textureFormat )
	{
		switch ( textureFormat )
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
			case DXGI_FORMAT_R8_UNORM: return 1;
			default: assert( false && "Unsupported texture format!" );
		}

		return 0;
	}
}
