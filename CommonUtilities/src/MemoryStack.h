/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _MEMORY_H_
#define _MEMORY_H_
// NOTE(pf): When allocation is made from the stack the currentLocation is returned, to reset stack 
// just return the marker that was given upon allocation.

class MemoryStack
{
public:
	MemoryStack();
	~MemoryStack();

	void ResetToMarker(MemoryMarker marker);

	void ZeroOutToMarker(MemoryMarker marker);

	// NOTE(pf): When an allocation is made the marker is moved, to reset before allocation just store what the current location was before and pass it to ResetTomarker.
	void *Allocate(MemoryMarker sizeInBytes);

	void *memory;
	MemoryMarker totalSize;
	MemoryMarker currentSize;
private:
};


#endif //!_MEMORY_H_
