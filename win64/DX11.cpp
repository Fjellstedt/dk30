/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11.h"
#include "DX11_Macros.h"
#include <fstream>

namespace Cryptic
{
	DX11::DX11()
	{
		m_swapChain = nullptr;
		m_device = nullptr;
		m_deviceContext = nullptr;
		m_renderTargetView = nullptr;
		m_depthStencilBuffer = nullptr;
		m_depthStencilState = nullptr;
		m_depthStencilView = nullptr;
		m_rasterState = nullptr;
	}

	DX11::~DX11()
	{
	}

	B32 DX11::Initialize(HWND hwnd, PlatformLayer *platLayer)
	{
		RenderSettings *settings = &platLayer->renderState.settings;
		IDXGIFactory *factory = nullptr;
		DX_HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory), L"DX11: Failed to create factory");

		IDXGIAdapter *adapters = nullptr;
		DX_HR(factory->EnumAdapters(0, &adapters), L"DX11: Failed to enumerate v.card adapters");

		IDXGIOutput *adapterOutput = nullptr;
		DX_HR(adapters->EnumOutputs(0, &adapterOutput), L"DX11: Failed to enumerate v.card outputs");

		U32 numModes = 0;
		// NOTE(pf): pre-fetch amount of modes..
		DX_HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, 0),
			  L"DX11: Failed to generate a display mode list.");

		MemoryMarker currentMM = platLayer->applicationMemory.currentSize;
		DXGI_MODE_DESC *displayModeList = platLayer->applicationMemory.Allocate<DXGI_MODE_DESC>(numModes);
		DX_HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList),
			  L"DX11: Failed to fetch display mode information");

		U32 numerator = 0, denominator = 0;
		for (U32 i = 0; i < numModes; ++i)
		{
			if (displayModeList[i].Width == (U32)settings->screenDim.x &&
				displayModeList[i].Height == (U32)settings->screenDim.y)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
				m_displayModes[0] = displayModeList[i];
			}
			else if (displayModeList[i].Width == 1920 &&
					 displayModeList[i].Height == 1080)
			{
				m_displayModes[1] = displayModeList[i];
			}
		}
		platLayer->applicationMemory.ResetToMarker(currentMM);
		displayModeList = nullptr;

		DX_HR((numerator == denominator == 0), L"DX11: Failed to fetch numerator and denominator from video card.");
		DX_HR(adapters->GetDesc(&m_videoCardDesc), L"DX11: Failed finding adapter desc.");

		DX_RELEASE(adapterOutput);
		DX_RELEASE(adapters);
		DX_RELEASE(factory);

		//int error;
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = settings->screenDim.x;
		swapChainDesc.BufferDesc.Height = settings->screenDim.y;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = true;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.Flags = 0;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		DX_HR(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION,
											&swapChainDesc, &m_swapChain, &m_device, 0, &m_deviceContext),
			  L"DX11: Failed to initialize swapchain, device and device context.");

		ID3D11Texture2D *backBufferPtr;
		DX_HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferPtr), L"DX11: Failed to reference a backbuffer.");

		DX_HR(m_device->CreateRenderTargetView(backBufferPtr, 0, &m_renderTargetView), L"DX11: Failed to create a render target view.");
		DX_RELEASE(backBufferPtr);

		D3D11_TEXTURE2D_DESC depthBufferDesc = {};
		depthBufferDesc.Width = settings->screenDim.x;
		depthBufferDesc.Height = settings->screenDim.y;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		DX_HR(m_device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer), L"DX11: Failed to create a depth stencil buffer.");

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		DX_HR(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState), L"DX11: Failed to create depth stencil state");

		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		DX_HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView),
			  L"DX11: Failed to create depth stencil view");

		// NOTE(pf): Binds our render target to the back buffer, so we render our graphics to the non displayed buffer.
		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		// NOTE(pf): Basicly the same raster state as the default one but allows for experimentation.
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		DX_HR(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), L"DX11: Failed to change raster state");

		D3D11_VIEWPORT viewport;
		viewport.Width = (F32)settings->screenDim.x;
		viewport.Height = (F32)settings->screenDim.y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		m_deviceContext->RSSetViewports(1, &viewport);

		F32 fov = PI / 4;
		F32 aspectRatio = (F32)settings->screenDim.x / (F32)settings->screenDim.y;
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio,
															   settings->m_screenN, settings->m_screenD);
		m_worldMatrix = DirectX::XMMatrixIdentity();
		m_orthoMatrix = DirectX::XMMatrixOrthographicLH((F32)settings->screenDim.x, (F32)settings->screenDim.y,
														settings->m_screenN, settings->m_screenD);
		
		if (!LoadBitmapFromFile(L"textures/uvtest.bmp", &platLayer->applicationMemory, &m_tmpTexture))
		{
			return false;
		}
		m_tmpModel.Initialize(m_device, "models/cube.txt", &platLayer->applicationMemory);
		m_tmpShader.Initialize(m_device, hwnd, L"shaders/hlsl/test_vs.hlsl", L"shaders/hlsl/test_ps.hlsl");
		return true;
	}

	void DX11::Shutdown()
	{
		DX_RELEASE(m_swapChain);
		DX_RELEASE(m_device);
		DX_RELEASE(m_deviceContext);
		DX_RELEASE(m_renderTargetView);
		DX_RELEASE(m_depthStencilBuffer);
		DX_RELEASE(m_depthStencilState);
		DX_RELEASE(m_depthStencilView);
		DX_RELEASE(m_rasterState);

		m_tmpModel.Shutdown();
		m_tmpShader.Shutdown();
		m_tmpTexture.Shutdown();
	}

	void DX11::ChangeFullscreenTo(bool bFlag)
	{
		m_swapChain->SetFullscreenState(bFlag, 0);
		m_swapChain->ResizeTarget(bFlag ? &m_displayModes[1] : &m_displayModes[0]);
	}

	B32 DX11::LoadBitmapFromFile(const wchar_t *fileName, MemoryStack *memory, DX11_Texture *target)
	{
		U8 *dataBuff[2] = {nullptr, nullptr};
		//U8 *pixels = nullptr;
		BITMAPFILEHEADER *bmpHeader = nullptr;
		BITMAPINFOHEADER *bmpInfo = nullptr;
		std::ifstream file(fileName, std::ios::binary);
		U32 imageSize = 0;

		// TODO(pf): logging.
		Assert(file && "We have no file error handling yet, file missing!!");

		dataBuff[0] = reinterpret_cast<U8 *>(memory->Allocate(sizeof(BITMAPFILEHEADER)));
		dataBuff[1] = reinterpret_cast<U8 *>(memory->Allocate(sizeof(BITMAPINFOHEADER)));
		file.read(reinterpret_cast<char *>(dataBuff[0]), sizeof(BITMAPFILEHEADER));
		file.read(reinterpret_cast<char *>(dataBuff[1]), sizeof(BITMAPINFOHEADER));
		bmpHeader = reinterpret_cast<BITMAPFILEHEADER *>(dataBuff[0]);
		bmpInfo = reinterpret_cast<BITMAPINFOHEADER *>(dataBuff[1]);

		imageSize = bmpInfo->biSizeImage;
		if (imageSize == 0)
		{
			imageSize = bmpHeader->bfSize - bmpHeader->bfOffBits;
		}

		U8 *unFlippedPixels = reinterpret_cast<U8 *>(memory->Allocate(imageSize));
		U8 *pixels = reinterpret_cast<U8 *>(memory->Allocate(imageSize));

		file.seekg(bmpHeader->bfOffBits);
		file.read((char *)unFlippedPixels, imageSize);
		for (I32 y = 0; y < bmpInfo->biHeight; ++y)
		{
			for (I32 x = 0; x < bmpInfo->biWidth; ++x)
			{
				U32 i = (y * bmpInfo->biWidth + x) * 4;
				U32 j = ((bmpInfo->biHeight - (y + 1)) * bmpInfo->biWidth + x) * 4;
				// NOTE(pf): Flipping Red and Blue and Multiplying alpha as we load it in.
				pixels[i + 0] = (U8)(unFlippedPixels[j + 2] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 1] = (U8)(unFlippedPixels[j + 1] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 2] = (U8)(unFlippedPixels[j + 0] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 3] = unFlippedPixels[j + 3];
				DEBUG_LEDGE;
			}
		}

		D3D11_TEXTURE2D_DESC bmpDesc = {};
		bmpDesc.Width = bmpInfo->biWidth;
		bmpDesc.Height = bmpInfo->biHeight;
		bmpDesc.MipLevels = 1;
		bmpDesc.ArraySize = 1;
		bmpDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bmpDesc.SampleDesc.Count = 1;
		bmpDesc.SampleDesc.Quality = 0;
		bmpDesc.Usage = D3D11_USAGE_DEFAULT;
		bmpDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bmpDesc.CPUAccessFlags = 0;
		bmpDesc.MiscFlags = 0;
		ID3D11Texture2D *tmp;
		D3D11_SUBRESOURCE_DATA pData = {};
		pData.pSysMem = pixels;
		pData.SysMemPitch = (bmpInfo->biWidth * (bmpInfo->biBitCount / 8));
		
		DX_HR(m_device->CreateTexture2D(&bmpDesc, &pData, &tmp), L"DX11: Failed to create a bmp texture.");

		return target->Initialize(m_device, tmp);
	}

	void DX11::Render(RenderState *state)
	{
		// Render pass
		m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::BLACK.e);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_tmpModel.Render(m_deviceContext);
		
		DirectX::XMVECTOR up = {0.0f, 1.0f, 0.0f, 0.0f}, 
			position = {state->groups->camera->pos.x, state->groups->camera->pos.y, state->groups->camera->pos.z, 1.0f},
			lookAt = {state->groups->camera->dir.x, state->groups->camera->dir.y, state->groups->camera->dir.z, 0.0f};

		F32 yaw = 0;
		F32 pitch = 0;
		F32 roll = 0;
		DirectX::XMMATRIX rotationMatrix;

		rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		lookAt = DirectX::XMVector3TransformCoord(lookAt, rotationMatrix);
		up = DirectX::XMVector3TransformCoord(up, rotationMatrix);
		lookAt = DirectX::XMVectorAdd(position, lookAt);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(position, lookAt, up);
#if true
		// Update the rotation variable each frame.
		static F32 rotation = (float)PI * 0.0015f;
		if (rotation > 360.0f)
		{
			rotation -= 360.0f;
		}
		m_worldMatrix *= DirectX::XMMatrixRotationY(rotation);
#endif
		m_tmpShader.Render(m_deviceContext, m_tmpModel.m_indexCount, m_tmpTexture.m_textureView, 
						   DirectX::XMFLOAT4(Colors::WHITE.e), DirectX::XMFLOAT4((Colors::BLUE * 0.15f).e), {0.f, 0.f, 1.f},
						   m_worldMatrix, view, m_projectionMatrix);
		// Swap Back Buffer
		if (state->settings.vSync)
			m_swapChain->Present(1, 0);
		else
			m_swapChain->Present(0, 0);
	}
}