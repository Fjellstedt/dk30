/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "w_pch.h"
#include "Win64_Renderer.h"
namespace Cryptic
{
    Win64_Renderer::Win64_Renderer()
    {
    }

    Win64_Renderer::~Win64_Renderer()
    {
    }

	void Win64_Renderer::Initialize(HWND hwnd, PlatformLayer *platLayer)
	{
        m_dx11.Initialize(hwnd, platLayer);
	}

    void Win64_Renderer::Render(RenderState *state)
    {
        // TODO(pf): Render API swapping.
        if (m_settings.fullscreen != state->settings.fullscreen)
            m_dx11.ChangeFullscreenTo(state->settings.fullscreen);

        m_settings = state->settings;

        m_dx11.Render(state);
    }
}