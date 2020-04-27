/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _RENDERING_H_
#define _RENDERING_H_

namespace Cryptic
{
	namespace Colors
	{
		const Math::V4f MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
		const Math::V4f WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
		const Math::V4f RED = {1.0f, 0.0, 0.0f, 1.0f};
		const Math::V4f GREEN = {0.0f, 1.0, 0.0f, 1.0f};
		const Math::V4f BLUE = {0.0f, 0.0, 1.0f, 1.0f};
	}

	struct DrawCall
	{
		DrawCall *next;
	};

	struct RenderSettings
	{
		B32 fullscreen;
		B32 vSync;
		Math::V2i screenDim;
		F32 m_screenD = 1000.f;
		F32 m_screenN = 0.01f;
	};

	struct RenderState
	{
		DrawCall *drawCalls;
		RenderSettings settings;
	};
}
#endif //!_RENDERING_H_