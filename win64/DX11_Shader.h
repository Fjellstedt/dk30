/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _DX11_SHADER_H_
#define _DX11_SHADER_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

namespace Cryptic
{
	class DX11_Shader
	{
	public:
		DX11_Shader();
		~DX11_Shader();

		B32 Initialize(ID3D11Device *device, HWND hwnd, const wchar_t *vsFileName, const wchar_t *psFileName);
		void Shutdown();
		B32 Render(ID3D11DeviceContext *context, int indexCount, DirectX::XMMATRIX world,
				   DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

	private:
		// TODO(pf): Is there a way to generalize the buffer layout for our shader ?
		struct MatrixBufferLayout
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
		};

		void OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, const wchar_t *shaderFilename);

		ID3D11VertexShader *m_vertexShader;
		ID3D11PixelShader *m_pixelShader;
		ID3D11InputLayout *m_layout;
		ID3D11Buffer *m_matrixBuffer;
	};
}

#endif //!_DX11_SHADER_H_