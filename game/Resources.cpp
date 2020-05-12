/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "g_pch.h"
#include "Resources.h"
#include <cctype>

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
		U8 *input = (U8 *)fileData.memory;
		while (!isdigit(*input)) { ++input; }
		char number[32];
		U32 index = 0;
		while (isdigit(*input))
		{
			number[index++] = *(input++);
		}
		number[index] = '\0';
		data->vertexCount = atoi(number);
		while (!isdigit(*input)) { ++input; }

		index = 0;
		while (isdigit(*input))
		{
			number[index++] = *(input++);
		}
		number[index] = '\0';
		data->indexCount = atoi(number);
		data->layout = m_memory.Allocate<ModelLayout>(data->vertexCount);
		while (!isdigit(*input)) { ++input; }
		F32 *modelData = (F32 *)input;
		for (int i = 0; i < (I32)data->vertexCount; ++i)
		{
			data->layout[i].x = *(modelData++);
			data->layout[i].y = *(modelData++);
			data->layout[i].z = *(modelData++);

			data->layout[i].tu = *(modelData++);
			data->layout[i].tv = *(modelData++);

			data->layout[i].nx = *(modelData++);
			data->layout[i].ny = *(modelData++);
			data->layout[i].nz = *(modelData++);
		}
		platLayer->FileFree(fileData.memory);
	}

	void Resources::LoadBitmapFromFile(const char *fileName, PlatformLayer *platLayer)
	{
		FileReadResult fileData = platLayer->FileLoad(fileName);
		U32 imageSize = 0;

		BitmapHeader *header = (BitmapHeader *)fileData.memory;

		imageSize = header->sizeOfBitmap;
		if (imageSize == 0)
		{
			imageSize = header->size - header->bitmapOffset;
		}
		/* TODO(pf): continue here.
		U8 *unFlippedPixels = reinterpret_cast<U8 *>(memory->Allocate(imageSize));
		U8 *pixels = reinterpret_cast<U8 *>(memory->Allocate(imageSize));

		file.seekg(bmpHeader->bfOffBits);
		file.read((char *)unFlippedPixels, imageSize);
		for (I32 y = 0; y < bmpInfo->biHeight; ++y)
		{
			for (I32 x = 0; x < bmpInfo->biWidth; ++x)
			{
				U32 i = (y * bmpInfo->biWidth + x) * 4;
				U32 j = ((bmpInfo->biHeight - (y + 1)) * bmpInfo->biWidth + x) * 4;
				// NOTE(pf): Flipping Red and Blue and Multiplying alpha as we load it in.
				pixels[i + 0] = (U8)(unFlippedPixels[j + 2] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 1] = (U8)(unFlippedPixels[j + 1] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 2] = (U8)(unFlippedPixels[j + 0] * MIN(unFlippedPixels[j + 3] / 255.0f, 1.0f));
				pixels[i + 3] = unFlippedPixels[j + 3];
				DEBUG_LEDGE;
			}
		}
		*/
	}
}