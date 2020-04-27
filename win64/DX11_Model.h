/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
namespace Cryptic
{
	class DX11_Model
	{
	public:
		DX11_Model();
		~DX11_Model();

		bool Initialize(ID3D11Device *device, U32 vertexCount, U32 indexCount);
		void Shutdown();
		void Render(ID3D11DeviceContext *context);

	private:
		// TODO(pf): Is there a way to generalize the vertex layout for our model representation ?
		struct VertexLayout
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 color;
		};
		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		U32 m_vertexCount, m_indexCount;
	};
}