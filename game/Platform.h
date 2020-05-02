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
		class Resources;
		class Camera;
		class Input;
		struct GameState
		{
			Resources *resources;
			Camera *camera;
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

			RenderState renderState;
			GameState gameState;
		};

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

#define GAME_ENTRY(name) void name(PlatformLayer *platformLayer)
		typedef GAME_ENTRY(GameEntryDLL);
#define GAME_LOOP(name) void name(PlatformLayer *platformLayer)
		typedef GAME_LOOP(GameLoopDLL);

#ifdef __cplusplus
	}
#endif
}
#endif //!_PLATFORM_H_
