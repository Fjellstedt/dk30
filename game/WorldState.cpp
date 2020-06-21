#include "g_pch.h"
#include "WorldState.h"

namespace Cryptic
{
	WorldState::WorldState()
	{
		testEntity = nullptr;
	}

	WorldState::~WorldState()
	{
	}

	void WorldState::Initialize()
	{
		m_entityPool.Initialize(m_memory.Allocate<Entity>(256), sizeof(Entity) * 256);
		camera = m_memory.AllocateAndInitialize<Camera>();
		camera->pos = {0,0,-10};
		camera->dir = Math::Direction3D::Forward;
	}

	Entity *WorldState::GetEntity()
	{
		Entity *result = m_entityPool.AllocateAndInitialize();
		return result;
	}

	void WorldState::ReturnEntity(Entity *entity)
	{
		m_entityPool.Free(entity);
	}
}