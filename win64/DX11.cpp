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
    }

    DX11::~DX11()
    {
    }

    B32 DX11::Initialize(HWND hwnd, PlatformLayer *platLayer)
    {
        RenderSettings *settings = &platLayer->renderState.settings;
        IDXGIFactory *factory = nullptr;
        DX_HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory), L"Failed to create factory");

        IDXGIAdapter *adapters = nullptr;
        DX_HR(factory->EnumAdapters(0, &adapters), L"Failed to enumerate v.card adapters");

        IDXGIOutput *adapterOutput = nullptr;
        DX_HR(adapters->EnumOutputs(0, &adapterOutput), L"Failed to enumerate v.card outputs");

        U32 numModes = 0;
        // NOTE(pf): pre-fetch amount of modes..
        DX_HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, 0),
              L"Failed to generate a display mode list.");

        MemoryMarker currentMM = platLayer->applicationMemory.currentSize;
        DXGI_MODE_DESC *displayModeList = platLayer->applicationMemory.Allocate<DXGI_MODE_DESC>(numModes);
        DX_HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList),
              L"Failed to fetch display mode information");

        U32 numerator = 0, denominator = 0;
        for (U32 i = 0; i < numModes; ++i)
        {
            if (displayModeList[i].Width == (U32)settings->screenDim.x &&
                displayModeList[i].Height == (U32)settings->screenDim.y)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
                break;
            }
        }
        platLayer->applicationMemory.ResetToMarker(currentMM);
        displayModeList = nullptr;

        DX_HR((numerator == denominator == 0), L"Failed to fetch numerator and denominator from video card.");
        DX_HR(adapters->GetDesc(&m_videoCardDesc), L"Failed finding adapter desc.");

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
              L"Failed to initialize swapchain, device and device context.");

        ID3D11Texture2D *backBufferPtr;
        DX_HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferPtr), L"Failed to reference a backbuffer.");
        
        DX_HR(m_device->CreateRenderTargetView(backBufferPtr, 0, &m_renderTargetView), L"Failed to create a render target view.");
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
        DX_HR(m_device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer), L"Failed to create a depth stencil buffer.");

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

        DX_HR(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState), L"Failed to create depth stencil state");

        m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
        DX_HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView),
              L"Failed to create depth stencil view");

        // NOTE(pf): Binds our render target to the back buffer, so we render our graphics to the non displayed buffer.
        m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

        // NOTE(pf): Basicly the same raster state as the default one but allows for experimentation.
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = false;
        rasterDesc.ScissorEnable = false;
        rasterDesc.SlopeScaledDepthBias = 0.0f;
        DX_HR(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), L"Failed to change raster state");

        D3D11_VIEWPORT viewport;
        m_deviceContext->RSSetViewports(1, &viewport);

        F32 fov = PI / 4;
        F32 aspectRatio = (F32)settings->screenDim.x / (F32)settings->screenDim.y;
        m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, 
                                                               settings->m_screenN, settings->m_screenD);
        m_worldMatrix = DirectX::XMMatrixIdentity();
        m_orthoMatrix = DirectX::XMMatrixOrthographicLH((F32)settings->screenDim.x, (F32)settings->screenDim.y,
                                                        settings->m_screenN, settings->m_screenD);
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
    }

    void DX11::ChangeFullscreenTo(bool bFlag)
    {
        m_swapChain->SetFullscreenState(bFlag, 0);
    }

    void DX11::Render(RenderState *state)
    {
        // Render pass
        m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::BLUE.e);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // Swap Back Buffer
        if(state->settings.vSync)
            m_swapChain->Present(1, 0);
        else
            m_swapChain->Present(0, 0);
    }
}