/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "g_pch.h"
#include "Resources.h"
#include <cctype>
#include <cstdio>

namespace Cryptic
{
#pragma pack(push, 1)
	struct BitmapHeader
	{
		U16 fileType;
		U32 fileSize;
		U16 reserved1;
		U16 reserved2;
		U32 bitmapOffset;
		U32 size;
		I32 width;
		I32 height;
		U16 planes;
		U16 bitsPerPixel;
		U32 compression;
		U32 sizeOfBitmap;
		I32 horzResolution;
		I32 vertResolution;
		U32 colorsUsed;
		U32 colorsImportant;

		U32 redMask;
		U32 greenMask;
		U32 blueMask;
	};
#pragma pack(pop)

	Resources::Resources()
	{
		m_modelIndex = 0;
		m_textureIndex = 0;
		resourcesLoaded = false;
	}

	Resources::~Resources()
	{
	}

	void Resources::Initialize()
	{
		m_modelData = m_memory.Allocate<ModelData>(256);
		m_textureData = m_memory.Allocate<TextureData>(256);
	}

	void Resources::LoadModel(const char *fileName, PlatformLayer *platLayer)
	{
		// TODO(pf): Interpreting values in text to integer/floating helper functions.
		ModelData *data = &m_modelData[m_modelIndex++];
		FileReadResult fileData = platLayer->FileLoad(fileName);
		I32 dataLeft = fileData.fileSize;
		U8 *input = (U8 *)fileData.memory;
		// NOTE(pf): .. skip header for now.
		ConsumeUntilNumber(&input, &dataLeft);
		
		data->vertexCount = ConsumeI32(&input, &dataLeft);

		ConsumeUntilNumber(&input, &dataLeft);

		data->indexCount = ConsumeI32(&input, &dataLeft);
		
		data->layout = m_memory.Allocate<ModelLayout>(data->vertexCount);
		ConsumeUntilNumber(&input, &dataLeft);
		for (int i = 0; i < (I32)data->vertexCount; ++i)
		{
			data->layout[i].x = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
			data->layout[i].y = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
			data->layout[i].z = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);

			data->layout[i].tu = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
			data->layout[i].tv = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);

			data->layout[i].nx = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
			data->layout[i].ny = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
			data->layout[i].nz = ConsumeF32(&input, &dataLeft);
			ConsumeUntilNumber(&input, &dataLeft);
		}
		platLayer->FileFree(fileData.memory);
	}

	void Resources::LoadBitmapFromFile(const char *fileName, PlatformLayer *platLayer)
	{
		TextureData *data = &m_textureData[m_textureIndex++];
		FileReadResult fileData = platLayer->FileLoad(fileName);
		U32 imageSize = 0;

		BitmapHeader *header = (BitmapHeader *)fileData.memory;
		data->height = header->height;
		data->width = header->width;
		data->stride = header->width * header->bitsPerPixel / 4;
		imageSize = header->sizeOfBitmap;
		if (imageSize == 0)
		{
			imageSize = header->fileSize - header->bitmapOffset;
		}
		
		//U8 *unFlippedPixels = reinterpret_cast<U8 *>(frameMemory->Allocate(imageSize));
		U32 *input = (U32 *)(((U8 *)fileData.memory) + header->bitmapOffset);
		data->pixels = (U8 *)(m_memory.Allocate(imageSize));
		I32 dataLeft = imageSize;
		for (I32 y = 0; y < header->height; ++y)
		{
			for (I32 x = 0; x < header->width; ++x)
			{
				U32 i = (y * header->width + x) * 4;
				U32 j = ((header->height - (y + 1)) * header->width + x) * 4;

				// NOTE(pf): Flipping Red and Blue and Multiplying alpha as we load it in.
				U32 pixelValue = *(input++);
				U8 red = (U8)(pixelValue >> 24);
				U8 green = (U8)(pixelValue >> 16);
				U8 blue = (U8)(pixelValue >> 8);
				U8 alpha = (U8)(pixelValue >> 0);
				data->pixels[i + 0] = (U8)(red * MIN(alpha / 255.0f, 1.0f));
				data->pixels[i + 1] = (U8)(green * MIN(alpha / 255.0f, 1.0f));
				data->pixels[i + 2] = (U8)(blue * MIN(alpha / 255.0f, 1.0f));
				data->pixels[i + 3] = alpha;
				DEBUG_LEDGE;
			}
		}
		platLayer->FileFree(fileData.memory);
	}

	I32 Resources::ConsumeI32(U8 **input, I32 *dataLeft)
	{
		U8 output[32];
		int index = 0;
		while (isdigit(**input) || **input == '-' && *dataLeft > 0)
		{
			output[index++] = *(*input)++;
			*dataLeft -= sizeof(**input);
		}
		output[index] = '\0';

		I32 result = atoi((char *)output);
		return result;
	}

	F32 Resources::ConsumeF32(U8 **input, I32 *dataLeft)
	{
		U8 output[32];
		int index = 0;
		while ((isdigit(**input) || **input == '.' || **input == ',' || **input == '-') && *dataLeft > 0)
		{
			output[index++] = *(*input)++;
			*dataLeft -= sizeof(**input);
		}
		output[index] = '\0';

		F32 result = (F32)atof((char *)output);
		return result;
	}
	
	void Resources::ConsumeUntilNumber(U8 **input, I32 *dataLeft)
	{
		while (!isdigit(**input) && *dataLeft > 0) 
		{ 
			++(*input); 
			*dataLeft -= sizeof(**input);
		}

		if (*dataLeft <= 0)
		{
			return;
		}
		// NOTE(pf): did we consume a negative sign ? revert then..
		U8 *checkPrev = *input;
		if (*(--checkPrev) == '-')
		{
			--(*input);
			*dataLeft += sizeof(**input);
		}
	}
}