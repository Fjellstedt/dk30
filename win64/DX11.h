/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#pragma once
#include <Rendering.h>

   // NOTE(pf): Lib linking.
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#define DX_RELEASE(a) if(a)\
{\
	a->Release(); \
	a = nullptr; \
}\

// TODO(pf): Logging system.
#define DX_HR(a, msg) if(FAILED((a))) \
{ \
	MessageBoxW(0, msg, L"Error", MB_OK);\
	return false;\
} \

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Platform.h>

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

		DirectX::XMMATRIX m_projectionMatrix;
		DirectX::XMMATRIX m_worldMatrix;
		DirectX::XMMATRIX m_orthoMatrix;
	};
}