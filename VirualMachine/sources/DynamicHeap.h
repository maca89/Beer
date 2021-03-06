#pragma once

#include "Heap.h"

namespace Beer
{
	class DynamicHeap : public Heap
	{
	protected:

		size_t mInitialSize;
		HANDLE mHandle;

	public:

		DynamicHeap(size_t initialSize);
		~DynamicHeap();

		virtual void init();

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount);
		virtual byte* alloc(uint32 size);

		//virtual void free(Object* obj);

	};
}