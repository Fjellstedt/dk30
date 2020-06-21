#ifndef _WORLD_STATE_H_
#define _WORLD_STATE_H_

#include "Entity.h"
#include "Rendering.h"

namespace Cryptic
{
	class WorldState
	{
	public:
		WorldState();
		~WorldState();

		void Initialize();

		Entity *GetEntity();
		void ReturnEntity(Entity *entity);


		MemoryStack m_memory;
		MemoryPool<Entity> m_entityPool;

		Camera *camera;
		Entity *testEntity;

	};
}
#endif //!_WORLD_STATE_H_