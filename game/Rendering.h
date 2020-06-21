/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _RENDERING_H_
#define _RENDERING_H_

#include "Transform.h"

namespace Cryptic
{
	namespace Colors
	{
		const Math::V4f MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
		const Math::V4f WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
		const Math::V4f BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
		const Math::V4f RED = {1.0f, 0.0, 0.0f, 1.0f};
		const Math::V4f GREEN = {0.0f, 1.0, 0.0f, 1.0f};
		const Math::V4f BLUE = {0.0f, 0.0, 1.0f, 1.0f};
	}

	struct ModelLayout
	{
		F32 x, y, z;
		F32 tu, tv;
		F32 nx, ny, nz;
	};

	struct ModelData
	{
		U32 indexCount, vertexCount;
		ModelLayout *layout;
	};

	struct TextureData
	{
		U32 width, height;
		U32 stride; //width * bytes per pixel.
		U8 *pixels;
	};

	struct DrawCall
	{
		U32 modelIndex;
		U32 modelTextureIndex;
		Transform transform;
		DrawCall *next;
	};

	class Camera
	{
	public:
		Math::V3f velocity;
		Math::V3f pos;

		Math::V3f dir;
	};

	struct RenderSettings
	{
		B32 fullscreen;
		B32 vSync;
		Math::V2i screenDim;
		F32 m_screenD = 1000.f;
		F32 m_screenN = 0.01f;
	};

	struct RenderGroup
	{
		DrawCall *drawCalls;
		Camera *camera;
		RenderGroup *next;
	};

	// STUDY(pf): Look into OGRE3Ds solution to map loaded textures into the platform renderer.
	struct Mapping
	{
		MemoryStack *frameMemory;
		ModelData *model;
		TextureData *texture;

		Mapping *next;
	};

	struct RenderState
	{
		Mapping *mappings;
		RenderGroup *groups;
		RenderSettings settings;
	};
}
#endif //!_RENDERING_H_