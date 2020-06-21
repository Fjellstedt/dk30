#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Transform.h"

// TODO(pf): Entity construction.

namespace Cryptic
{
	class Entity
	{
	public:
		Entity();
		~Entity();

		U32 modelIndex;
		U32 textureIndex;
		Transform transform;
		F32 rotation;
	};

}
#endif //!_ENTITY_H_