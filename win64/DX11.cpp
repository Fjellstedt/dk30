/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11.h"
#include "DX11_Macros.h"

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

		m_currentFreeModel = 0;
		m_currentFreeTexture = 0;
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
		m_tmpShader.Initialize(m_device, hwnd, L"shaders/hlsl/test_vs.hlsl", L"shaders/hlsl/test_ps.hlsl");

		return true;
	}

	U32 DX11::MapModel(ModelData *data, MemoryStack *frameMemory)
	{
		U32 result = m_currentFreeModel++;
		m_models[result].Initialize(m_device, data, frameMemory);
		return result;
	}

	U32 DX11::MapTexture(TextureData *data, MemoryStack *frameMemory)
	{
		U32 result = m_currentFreeTexture++;
		D3D11_TEXTURE2D_DESC bmpDesc = {};
		bmpDesc.Width = data->width;
		bmpDesc.Height = data->height;
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
		pData.pSysMem = data->pixels;
		pData.SysMemPitch = data->stride;
		DX_HR(m_device->CreateTexture2D(&bmpDesc, &pData, &tmp), L"DX11: Failed to create a bmp texture.");
		m_textures[result].Initialize(m_device, tmp);

		return result;
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

		for (DX11_Model *model = m_models; model->IsValid(); model++)
		{
			model->Shutdown();
		}
		for (DX11_Texture *texture = m_textures; texture->IsValid(); texture++)
		{
			texture->Shutdown();
		}

		m_tmpShader.Shutdown();
	}

	void DX11::ChangeFullscreenTo(bool bFlag)
	{
		m_swapChain->SetFullscreenState(bFlag, 0);
		m_swapChain->ResizeTarget(bFlag ? &m_displayModes[1] : &m_displayModes[0]);
	}

	void DX11::Render(RenderState *state)
	{
		// Mapping
		for (Mapping *mapping = state->mappings; mapping != nullptr; mapping = mapping->next)
		{
			if (mapping->model) MapModel(mapping->model, mapping->frameMemory);
			if (mapping->texture) MapTexture(mapping->texture, mapping->frameMemory);
		}

		// Render pass
		m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::BLACK.e);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		for (RenderGroup *group = state->groups; group != nullptr; group = group->next)
		{
			for (DrawCall *drawCall = group->drawCalls; drawCall != nullptr; drawCall = drawCall->next)
			{
				Transform *transform = &drawCall->transform;
				DX11_Model *model = &m_models[drawCall->modelIndex];
				DX11_Texture *texture = &m_textures[drawCall->modelTextureIndex];

				if (!model->IsValid() || !texture->IsValid()) continue;

				model->Render(m_deviceContext);

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
				m_worldMatrix = DirectX::XMMATRIX(transform->matrix.e); // TODO(pf): Better conversion method ?
				m_tmpShader.Render(m_deviceContext, model->m_data->indexCount, texture->m_textureView,
								   DirectX::XMFLOAT4(Colors::WHITE.e), DirectX::XMFLOAT4((Colors::BLUE * 0.15f).e), {0.f, 0.f, 1.f},
								   m_worldMatrix, view, m_projectionMatrix);

			}
		}
		// Swap Back Buffer
		if (state->settings.vSync)
			m_swapChain->Present(1, 0);
		else
			m_swapChain->Present(0, 0);
	}
}