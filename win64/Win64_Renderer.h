/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#pragma once
#include <Rendering.h>
#include "DX11.h"
#include <Platform.h>

namespace Cryptic
{
	class Win64_Renderer
	{
	public:
		Win64_Renderer();
		~Win64_Renderer();

		void Initialize(HWND hwnd, PlatformLayer *platLayer);
		void Render(RenderState *state);

	private:
		RenderSettings m_settings;
		DX11 m_dx11;
	};
}
