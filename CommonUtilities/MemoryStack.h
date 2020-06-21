/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _MEMORY_STACK_H_
#define _MEMORY_STACK_H_
// NOTE(pf): When allocation is made from the stack the currentLocation is returned, to reset stack 
// just return the marker that was given upon allocation.

// TODO(pf): Memory alignment.

class MemoryStack
{
public:
	MemoryStack();
	~MemoryStack();

	void ResetToMarker(MemoryMarker marker);

	void ZeroOutToMarker(MemoryMarker marker);

	// NOTE(pf): When an allocation is made the marker is moved, 
	// to reset before allocation just store what the current location was before and pass it to ResetTomarker.
	// Alignment must be power of 2.
	void *Allocate(MemoryMarker sizeInBytes);
	template<typename T>
	T *Allocate(U32 count = 1)
	{
		MemoryMarker sizeInBytes = sizeof(T) * count;
		Assert(currentSize + sizeInBytes <= totalSize);
		void *result = reinterpret_cast<U8 *>(memory) + (currentSize);
		currentSize += sizeInBytes;
		return (T *)result;
	}

	template<typename T>
	T *AllocateAndInitialize(U32 count = 1)
	{
		T *result = (T *)Allocate<T>(count);
		*result = {};
		return result;
	}

	void *memory;
	MemoryMarker totalSize;
	MemoryMarker currentSize;

private:
};


#endif //!_MEMORY_STACK_H_
