/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "g_pch.h"
#include "Platform.h"
#include "Input.h"
#include "Rendering.h"

#include <Windows.h>

namespace Cryptic
{
	struct GameState
	{
		Input input;
		MemoryStack gameMemory;
		MemoryStack permanentMemory;
		MemoryStack frameMemory;
	};

#pragma comment(linker, "/export:GameEntry")
	extern "C" GAME_ENTRY(GameEntry)
	{
		GameState *gameState = (GameState *)platformLayer->memory;
		gameState->gameMemory = {};
		gameState->gameMemory.totalSize = platformLayer->memorySize - sizeof(GameState);
		gameState->gameMemory.memory = (U8 *)platformLayer->memory + sizeof(GameState);

		gameState->permanentMemory = {};
		gameState->permanentMemory.totalSize = MB(20);
		gameState->permanentMemory.memory = gameState->gameMemory.Allocate(gameState->permanentMemory.totalSize);

		gameState->frameMemory = {};
		gameState->frameMemory.totalSize = MB(200);
		gameState->frameMemory.memory = gameState->gameMemory.Allocate(gameState->frameMemory.totalSize);
	}

#pragma comment(linker, "/export:GameLoop")
	extern "C" GAME_LOOP(GameLoop)
	{
		GameState *gameState = (GameState *)platformLayer->memory;
		// NOTE(PF): Continue to make it so we can zero out memory.
		gameState->frameMemory.ZeroOutToMarker(0);
		gameState->input.state = inputState;
		if (gameState->input.GetButtonPressed(SpecialButton::M1))
		{
			OutputDebugStringA("Hej M2\n");
		}

		if (gameState->input.GetButtonPressed(SpecialButton::M1))
		{
			OutputDebugStringA("Hej M4\n");
		}
	}
}