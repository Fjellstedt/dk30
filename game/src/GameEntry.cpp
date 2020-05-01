/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "g_pch.h"
#include "Platform.h"
#include "Input.h"
#include "Rendering.h"

namespace Cryptic
{
#pragma comment(linker, "/export:GameEntry")
	extern "C" GAME_ENTRY(GameEntry)
	{
		// NOTE(pf): Memory delegation.
		GameState *gameState = &platformLayer->gameState;
		gameState->gameMemory = {};
		gameState->gameMemory.totalSize = platformLayer->applicationMemory.totalSize - platformLayer->applicationMemory.currentSize;
		gameState->gameMemory.memory = platformLayer->applicationMemory.Allocate(gameState->gameMemory.totalSize);

		gameState->permanentMemory = {};
		gameState->permanentMemory.totalSize = MB(50);
		gameState->permanentMemory.memory = gameState->gameMemory.Allocate(gameState->permanentMemory.totalSize);

		gameState->frameMemory = {};
		gameState->frameMemory.totalSize = MB(200);
		gameState->frameMemory.memory = gameState->gameMemory.Allocate(gameState->frameMemory.totalSize);
	}

#pragma comment(linker, "/export:GameLoop")
	extern "C" GAME_LOOP(GameLoop)
	{
		// NOTE(PF): Continue to make it so we can zero out memory.
		platformLayer->gameState.frameMemory.ZeroOutToMarker(0);
		GameState *gameState = &platformLayer->gameState;
		if (gameState->input.GetButtonPressed('F'))
		{
			platformLayer->renderState.settings.fullscreen = !platformLayer->renderState.settings.fullscreen;
		}
		if (gameState->input.GetButtonPressed('V'))
		{
			platformLayer->renderState.settings.vSync = !platformLayer->renderState.settings.vSync;
		}
	}
}