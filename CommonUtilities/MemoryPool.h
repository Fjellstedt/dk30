/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

   // NOTE(pf): The memory stored in the pool must exceed the minimum size of a pointer. When memory is freed that adress is just
   // reinterpreted into an adress that is chained together.

template<typename T>
class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();

	void Initialize(void *memory, U32 totalSize);

	T *Allocate();
	T *AllocateAndInitialize();

	void Free(void *memory);

private:
	struct FreeChain
	{
		FreeChain *next;
	};

	void *memory;
	U32 totalSize;
	U32 currentSize;
	FreeChain *firstFree;
};
#endif //!_MEMORY_POOL_H_

template<typename T>
inline MemoryPool<T>::MemoryPool()
{
#if _DEBUG
	U32 sizeOfArgument = sizeof(T);
	Assert(sizeOfArgument >= sizeof(size_t));
#endif
	firstFree = nullptr;
	totalSize = 0;
	currentSize = 0;
	memory = nullptr;
}

template<typename T>
inline MemoryPool<T>::~MemoryPool()
{
	firstFree = nullptr;
}

template<typename T>
inline void MemoryPool<T>::Initialize(void *data, U32 totalSizeInBytes)
{
	memory = data;
	totalSize = totalSizeInBytes;
}

template<typename T>
inline T *MemoryPool<T>::Allocate()
{
	if (firstFree)
	{
		T *result = (T *)firstFree;
		firstFree = firstFree->next;
		return result;
	}

	Assert((currentSize + sizeof(T)) <= totalSize);
	T *result = (T *) ((U8 *)memory + currentSize);
	currentSize += sizeof(T);
	return result;
}

template<typename T>
inline T *MemoryPool<T>::AllocateAndInitialize()
{
	T *result = (T *)Allocate();
	*result = {};
	return result;
}

template<typename T>
inline void MemoryPool<T>::Free(void *data)
{
	if (firstFree)
	{
		FreeChain *prev = firstFree;
		firstFree = (FreeChain *)data;
		firstFree->next = prev;
	}
	else
	{
		firstFree = (FreeChain *)data;
	}
}
