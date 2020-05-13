/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _WIN64_RENDERER_H_
#define _WIN64_RENDERER_H_
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

		B32 Initialize(HWND hwnd, PlatformLayer *platLayer);
		void Shutdown();
		void Render(RenderState *state);

		DX11 m_dx11;
	private:
		RenderSettings m_settings;
	};
}
#endif //!_WIN64_RENDERER_H_