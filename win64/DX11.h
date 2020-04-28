/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _DX11_H_
#define _DX11_H_
#include <Rendering.h>

   // NOTE(pf): Lib linking.
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Platform.h>
#include "DX11_Model.h"
#include "DX11_Shader.h"

namespace Cryptic
{
	class DX11
	{
	public:
		DX11();
		~DX11();
		B32 Initialize(HWND hwnd, PlatformLayer *platLayer);
		void Render(RenderState *renderState);
		void Shutdown();

		void ChangeFullscreenTo(bool bFlag);

		DXGI_ADAPTER_DESC m_videoCardDesc;
	private:

		IDXGISwapChain *m_swapChain;
		ID3D11Device *m_device;
		ID3D11DeviceContext *m_deviceContext;
		ID3D11RenderTargetView *m_renderTargetView;
		ID3D11Texture2D *m_depthStencilBuffer;
		ID3D11DepthStencilState *m_depthStencilState;
		ID3D11DepthStencilView *m_depthStencilView;
		ID3D11RasterizerState *m_rasterState;

		DXGI_MODE_DESC m_displayModes[2];

		DX11_Model m_tmpModel;
		DX11_Shader m_tmpShader;
		DirectX::XMMATRIX m_projectionMatrix;
		DirectX::XMMATRIX m_worldMatrix;
		DirectX::XMMATRIX m_orthoMatrix;
	};
}

#endif //!_DX11_H_