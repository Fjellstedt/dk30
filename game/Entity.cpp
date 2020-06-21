#include "g_pch.h"
#include "Entity.h"

namespace Cryptic
{
	Entity::Entity()
	{
		modelIndex = 0;
		textureIndex = 0;
		transform = {};
		rotation = 0.f;
	}

	Entity::~Entity()
	{
	}
}