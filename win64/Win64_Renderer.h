/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#pragma once
#include <Rendering.h>
namespace Cryptic
{
	class Win64_Renderer
	{
	public:
		Win64_Renderer();
		~Win64_Renderer();

		void BeginDraw(DrawCall *drawCalls);
		void EndDraw();

		Math::V2i screenDim;
	private:
	};
}
