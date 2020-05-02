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

		gameState->input = gameState->permanentMemory.Allocate<Input>();
		gameState->camera = gameState->permanentMemory.Allocate<Camera>();
		*gameState->camera = {};
		gameState->camera->pos = {0,0,-10};
		gameState->camera->dir = Math::Direction3D::Forward;

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
		RenderState *renderState = &platformLayer->renderState;
		if (gameState->input->GetButtonPressed('F'))
		{
			platformLayer->renderState.settings.fullscreen = !platformLayer->renderState.settings.fullscreen;
		}
		if (gameState->input->GetButtonPressed('V'))
		{
			platformLayer->renderState.settings.vSync = !platformLayer->renderState.settings.vSync;
		}
		
		// Camera movment:
		// Translation
		Camera *camera = gameState->camera;
		Math::V3f moveAcc = {};
		if (gameState->input->GetButtonHeld('W'))
		{
			moveAcc += camera->dir;
		}
		if (gameState->input->GetButtonHeld('A'))
		{
			moveAcc += -Math::V3f::Cross(Math::Direction3D::Up, camera->dir);
		}
		if (gameState->input->GetButtonHeld('S'))
		{
			moveAcc += -camera->dir;
		}
		if (gameState->input->GetButtonHeld('D'))
		{
			moveAcc += Math::V3f::Cross(Math::Direction3D::Up, camera->dir);
		}

		if (moveAcc.LengthSq() > 0)
		{
			moveAcc.Normalize();
			moveAcc *= 0.3f;
			camera->velocity += moveAcc;
		}

		// NOTE(pf): Hacky drag to remove velocity.
		Math::V3f drag = camera->velocity * 0.4f;
		if (Math::V3f::LengthSq(drag) >= Math::V3f::LengthSq(camera->velocity))
		{
			camera->velocity = {};
		}
		else
		{
			camera->velocity -= drag;
		}
		camera->pos += camera->velocity;

		// TODO(pf): Rotation...
		renderState->groups->camera = camera;
	}
}