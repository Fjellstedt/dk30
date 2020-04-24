/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "Rendering.h"
#include "Input.h"
namespace Cryptic
{
#ifdef __cplusplus
	extern "C" {
#endif
		struct PlatformLayer
		{
			void *memory;
			U64 memorySize;
			const F32 pixelsPerMeter;
			B32 reloaded;
			B32 shouldExitGame;
			U32 screenWidth;
			U32 screenHeight;
			F32 delta;
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
#define GAME_LOOP(name) void name(PlatformLayer *platformLayer, DrawCall *drawCalls, Input::InputState *inputState)
		typedef GAME_LOOP(GameLoopDLL);

#ifdef __cplusplus
	}
#endif
}
#endif //!_PLATFORM_H_
