/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _DX11_MODEL_H_
#define _DX11_MODEL_H_
#include <DirectXMath.h>
#include <Rendering.h>
#include <d3d11.h>
namespace Cryptic
{
	class DX11_Model
	{
	public:
		DX11_Model();
		~DX11_Model();

		bool Initialize(ID3D11Device *device, const char *fileName, MemoryStack *memory);
		void Shutdown();
		void Render(ID3D11DeviceContext *context);

		U32 m_vertexCount, m_indexCount;
	private:
		// TODO(pf): Is there a way to generalize the vertex layout for our model representation ?
		struct VertexLayout
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 normal;
		};

		// TODO(pf): Move this into app resource side.
		struct ModelLayout
		{
			F32 x, y, z;
			F32 tu, tv;
			F32 nx, ny, nz;
		};

		B32 LoadModel(const char *fileName, MemoryStack *memory);

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		ModelLayout *m_modelLayout;
	};
}
#endif //!_DX11_MODEL_H_