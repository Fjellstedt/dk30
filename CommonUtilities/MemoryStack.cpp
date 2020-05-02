/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "cu_pch.h"
#include "MemoryStack.h"

MemoryStack::MemoryStack()
{
	currentSize = 0;
	totalSize = 0;
}

MemoryStack::~MemoryStack()
{
}

void MemoryStack::ResetToMarker(MemoryMarker marker)
{
	currentSize = marker;
}

void MemoryStack::ZeroOutToMarker(MemoryMarker marker)
{
#if SLOW_CODE
	int iterations = 0;
#endif
	for (U8 *p = ((U8 *)(memory)+marker); p != ((U8 *)(memory)+currentSize); p++)
	{
		*p = 0;
#if SLOW_CODE
		++iterations;
	}
	Assert(iterations == (currentSize - marker) / sizeof(U8));
#else 
	}
#endif
	currentSize = marker;
}

// NOTE(pf): When an allocation is made the marker is moved, to reset before allocation just store what the current location was before and pass it to ResetTomarker.
void *MemoryStack::Allocate(MemoryMarker sizeInBytes)
{
	Assert(currentSize + sizeInBytes <= totalSize);
	void *result = reinterpret_cast<U8 *>(memory) + (currentSize);
	currentSize += sizeInBytes;
	return result;
}