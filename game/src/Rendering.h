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

	struct LoadedBitmap
	{
		U64 bitmapHandle;
		Math::Rect2f rect;
	};

	struct DrawCall_Text
	{
		Math::V2f pos;
		W16 txt[256];
		I32 charCount;
	};

	struct DrawCall_Bitmap
	{
		LoadedBitmap *bitmap;
		Math::V2f pos;
		Math::V2f dimensions;
		F32 rot;
	};

	struct DrawCall_Brush
	{
		DrawCall_Brush()
		{
			shouldFill = false;
			outlineColor = Colors::MAGENTA;
			fillColor = Colors::MAGENTA;
		}
		Math::V4f outlineColor;
		F32 outlineWidth;
		Math::V4f fillColor;
		Math::Rect2f rect;
		B32 shouldFill;
	};

	// NOTE(pf): Temp solution to add variety to our drawcalls, should NOT be a long list of different types.
	struct DrawCall
	{
		DrawCall_Bitmap *bmpCall;
		DrawCall_Text *txtCall;
		DrawCall_Brush *brushCall;
		DrawCall *Next;
	};

	struct RenderSettings
	{
		B32 fullscreen;
		B32 vSync;
		Math::V2i screenDim;
		F32 m_screenD = 1000.f, m_screenN = 0.01f;
		F32 pixelsPerMeter;
	};

	struct RenderState
	{
		DrawCall *drawCalls;
		RenderSettings settings;
	};
}
#endif //!_RENDERING_H_