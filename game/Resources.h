/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include "Rendering.h"
#include "Platform.h"

namespace Cryptic
{
	class Resources
	{
	public:
		Resources();
		~Resources();

		void Initialize();

		void LoadModel(const char *fileName, PlatformLayer *platLayer);
		void LoadBitmapFromFile(const char *fileName, PlatformLayer *platLayer);
		
		MemoryStack m_memory;

		ModelData *m_modelData;
		U32 m_modelIndex;
		TextureData *m_textureData;
		U32 m_textureIndex;
	private:
	};
}
#endif //!_RESOURCES_H_