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
		U32 m_modelCount;

		TextureData *m_textureData;
		U32 m_textureIndex;
		U32 m_textureCount;

		B32 resourcesLoaded;
	private:
		//TODO(pf): Move this text interpretation somewhere else, currently in use here to convert the loaded memory into resource data.
		I32 ConsumeI32(U8 **input, I32 *dataLeft);
		F32 ConsumeF32(U8 **input, I32 *dataLeft);
		void ConsumeUntilNumber(U8 **input, I32 *dataLeft);
	};
}
#endif //!_RESOURCES_H_