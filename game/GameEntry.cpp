/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "g_pch.h"
#include "Platform.h"
#include "Input.h"
#include "Rendering.h"
#include "Resources.h"

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

		gameState->input = gameState->permanentMemory.AllocateAndInitialize<Input>();
		gameState->camera = gameState->permanentMemory.AllocateAndInitialize<Camera>();
		gameState->camera->pos = {0,0,-10};
		gameState->camera->dir = Math::Direction3D::Forward;

		gameState->frameMemory = {};
		gameState->frameMemory.totalSize = MB(200);
		gameState->frameMemory.memory = gameState->gameMemory.Allocate(gameState->frameMemory.totalSize);

		gameState->resources = gameState->permanentMemory.AllocateAndInitialize<Resources>();
		gameState->resources->m_memory.totalSize = MB(30);
		gameState->resources->m_memory.memory = gameState->permanentMemory.Allocate(gameState->resources->m_memory.totalSize);
		gameState->resources->Initialize();
	}

#pragma comment(linker, "/export:GameLoop")
	extern "C" GAME_LOOP(GameLoop)
	{
		// NOTE(PF): Continue to make it so we can zero out memory..
		platformLayer->gameState.frameMemory.ZeroOutToMarker(0);
		GameState *gameState = &platformLayer->gameState;
		RenderState *renderState = &platformLayer->renderState;
		// NOTE(pf): .. then cleanup for this frame.
		renderState->mappings = nullptr;
		renderState->groups = nullptr;
		
		if (gameState->input->GetButtonPressed('F'))
		{
			platformLayer->renderState.settings.fullscreen = !platformLayer->renderState.settings.fullscreen;
		}
		if (gameState->input->GetButtonPressed('V'))
		{
			platformLayer->renderState.settings.vSync = !platformLayer->renderState.settings.vSync;
		}
		if (gameState->input->GetButtonPressed('L'))
		{
			gameState->resources->LoadModel("models/cube.txt", platformLayer);
			//gameState->resources->LoadBitmapFromFile("textures/structured_art.bmp", platformLayer);
			gameState->resources->LoadBitmapFromFile("textures/uvtest.bmp", platformLayer);
			renderState->mappings = gameState->frameMemory.AllocateAndInitialize<Mapping>();
			renderState->mappings->frameMemory = &gameState->frameMemory;
			renderState->mappings->model = &gameState->resources->m_modelData[0];
			renderState->mappings->texture = &gameState->resources->m_textureData[0];
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

		// NOTE(pf): Temp drawcall generation.
		renderState->groups = gameState->frameMemory.AllocateAndInitialize<RenderGroup>();

		renderState->groups->camera = camera;

		renderState->groups->drawCalls = gameState->frameMemory.AllocateAndInitialize<DrawCall>();
		renderState->groups->drawCalls->modelIndex = 0;
		renderState->groups->drawCalls->modelTextureIndex = 0;
		renderState->groups->drawCalls->transform.pos = {};
		renderState->groups->drawCalls->next = nullptr;
	}
}