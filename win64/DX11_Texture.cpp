/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11_Texture.h"
#include "DX11_Macros.h"

namespace Cryptic
{
    DX11_Texture::DX11_Texture()
    {
    }

    DX11_Texture::~DX11_Texture()
    {
    }

    B32 DX11_Texture::Initialize(ID3D11Device *device, ID3D11Texture2D *texture)
    {
        DX_HR(device->CreateShaderResourceView(texture, 0, &m_textureView), L"DX11: Failed to create a shader resource view.");
        return true;
    }

    void DX11_Texture::Shutdown()
    {
        DX_RELEASE(m_textureView);
    }
}