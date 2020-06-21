/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "Rendering.h"
namespace Cryptic
{
#ifdef __cplusplus
	extern "C" {
#endif
		struct FileReadResult
		{
			U32 fileSize;
			void *memory;
		};
#define PLAT_FILE_LOAD(name) FileReadResult name(const char *fileName)
		typedef PLAT_FILE_LOAD(PlatformFileLoad);

#define PLAT_FILE_FREE(name) void name(void *memory)
		typedef PLAT_FILE_FREE(PlatformFileFree);

#define PLAT_RENDERER_MAP_MODEL(name) U32 name(ModelData *data, MemoryStack *frameMemory)
		typedef PLAT_RENDERER_MAP_MODEL(PlatformRendererMapModel);

#define PLAT_RENDERER_MAP_TEXTURE(name) U32 name(TextureData *data, MemoryStack *frameMemory)
		typedef PLAT_RENDERER_MAP_TEXTURE(PlatformRendererMapTexture);

		class Resources;
		class Camera;
		class Input;
		class WorldState;
		struct GameState
		{
			WorldState *worldState;
			Resources *resources;
			Input *input;
			MemoryStack gameMemory;
			MemoryStack permanentMemory;
			MemoryStack frameMemory;
		};

		// NOTE(pf): This is just the quickest way to start up, might be a bad idea to 
		// store all platform related settings in one place.
		struct PlatformLayer
		{
			MemoryStack applicationMemory;
			B32 shouldExitGame;
			F32 delta;
			
			PlatformFileLoad *FileLoad;
			PlatformFileFree *FileFree;

			RenderState renderState;
			GameState gameState;
		};

#define GAME_ENTRY(name) void name(PlatformLayer *platformLayer)
		typedef GAME_ENTRY(GameEntryDLL);

#define GAME_LOOP(name) void name(PlatformLayer *platformLayer)
		typedef GAME_LOOP(GameLoopDLL);

#ifdef _WINDOWS
		enum SpecialButton
		{
			ArrowKeyUp = 0x26,
			ArrowKeyLeft = 0x25,
			ArrowKeyDown = 0x28,
			ArrowKeyRight = 0x27,
			SPACE = 0x20,
			M1 = 0x01,
			M2 = 0x02,
		};
#else
		enum SpecialButton
		{
			ArrowKeyUp,
			ArrowKeyLeft,
			ArrowKeyDown,
			ArrowKeyRight,
			SPACE,
			M1,
			M2,
		};
#endif

		inline U32 SafeTruncateU64(U64 value)
		{
			Assert(value <= 0xFFFFFFFF);
			U32 result = (U32)value;
			return result;
		}

#ifdef __cplusplus
	}
#endif
}
#endif //!_PLATFORM_H_
