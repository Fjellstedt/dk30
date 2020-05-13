/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _DX11_TEXTURE_H_
#define _DX11_TEXTURE_H_

#include <d3d11.h>


namespace Cryptic
{
	class DX11_Texture
	{
	public:
		DX11_Texture();
		~DX11_Texture();

		B32 Initialize(ID3D11Device *device, ID3D11Texture2D *texture);
		void Shutdown();

		ID3D11ShaderResourceView *m_textureView;
		B32 IsValid() { return m_textureView != nullptr; }
	private:
	};
}
#endif //!_DX11_TEXTURE_H_