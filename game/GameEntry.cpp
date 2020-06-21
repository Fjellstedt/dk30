/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "g_pch.h"
#include "Platform.h"
#include "Input.h"
#include "Rendering.h"
#include "Resources.h"
#include "WorldState.h"

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

		gameState->worldState = gameState->permanentMemory.AllocateAndInitialize<WorldState>();
		gameState->worldState->m_memory.totalSize = MB(10);
		gameState->worldState->m_memory.memory = gameState->permanentMemory.Allocate(gameState->worldState->m_memory.totalSize);
		gameState->worldState->Initialize();

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
		WorldState *worldState = gameState->worldState;
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

			Entity *first = worldState->GetEntity();
			first->modelIndex = 56;
			Entity *sec = worldState->GetEntity();
			sec->modelIndex = 32;
			worldState->ReturnEntity(first);
			first = worldState->GetEntity();
			worldState->ReturnEntity(first);
			worldState->ReturnEntity(sec);
			first = worldState->GetEntity();
			worldState->testEntity = worldState->GetEntity();
		}

		// Camera movment:
		// Translation
		Camera *camera = worldState->camera;
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

		if (worldState->testEntity)
		{
			Math::V3f pos = {};
			if (gameState->input->GetButtonHeld(SpecialButton::ArrowKeyLeft))
			{
				pos.x -= 1;
			}
			if (gameState->input->GetButtonHeld(SpecialButton::ArrowKeyRight))
			{
				pos.x += 1;
			}
			if (gameState->input->GetButtonHeld(SpecialButton::ArrowKeyUp))
			{
				pos.y += 1;
			}
			if (gameState->input->GetButtonHeld(SpecialButton::ArrowKeyDown))
			{
				pos.y -= 1;
			}

			worldState->testEntity->transform.pos += pos;
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


		if (worldState->testEntity != nullptr)
		{
			// NOTE(pf): Temp drawcall generation.
			renderState->groups = gameState->frameMemory.AllocateAndInitialize<RenderGroup>();

			renderState->groups->camera = camera;

			renderState->groups->drawCalls = gameState->frameMemory.AllocateAndInitialize<DrawCall>();

			DrawCall *drawCall = renderState->groups->drawCalls;
			renderState->groups->drawCalls->modelIndex = worldState->testEntity->modelIndex;
			renderState->groups->drawCalls->modelTextureIndex = worldState->testEntity->textureIndex;
			renderState->groups->drawCalls->transform = worldState->testEntity->transform;
			renderState->groups->drawCalls->next = nullptr;
		}
	}
}