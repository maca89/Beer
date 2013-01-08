#pragma once
#include "prereq.h"


namespace Beer
{
	class SimpleMemoryAllocator
	{
	public:
		typedef uint32 length_t;
		//typedef uint64 length_t;
		enum { SIZEOF_HEAD = sizeof(length_t) };

	protected:
		#pragma pack(push, 1)
		struct Node
		{
			length_t size;
			byte* ptr;
		};
		#pragma pack(pop)

		HANDLE mHeap;
		byte* mHead;
		length_t mSize;
		length_t mNext;

	public:
		INLINE SimpleMemoryAllocator() : mHead(NULL), mSize(0), mNext(0), mHeap(NULL) {}
		INLINE SimpleMemoryAllocator(length_t length) : mHead(NULL), mHeap(NULL), mSize(0), mNext(0) { init(length); }
		INLINE ~SimpleMemoryAllocator() { destroy(); }

		NOINLINE void init(length_t length)
		{
			mSize = length;
			mHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, length, 0);
			mHead = static_cast<byte*>(HeapAlloc(mHeap, HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, length));
			clear();
		}

		INLINE void destroy()
		{
			HeapFree(mHeap, HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, mHead);
			HeapDestroy(mHeap);
			mSize = 0;
			mHead = NULL;
			mHeap = NULL;
		}

		INLINE void* malloc(length_t size)
		{
			if(!canMalloc(size))
			{
				return NULL;
			}

			Node* node = reinterpret_cast<Node*>(&mHead[mNext]);
			node->size = size;
			mNext += SIZEOF_HEAD + size;

			return &node->ptr;
		}

		INLINE bool canMalloc(length_t size) const
		{
			return mNext + size + SIZEOF_HEAD < mSize;
		}

		INLINE void* getFirstNode() const
		{
			return mHead + SIZEOF_HEAD;
		}

		INLINE void* getNextNode(void* node) const
		{
			byte* next = static_cast<byte*>(node) + getNodeSize(node) + SIZEOF_HEAD;

			if(next >= &mHead[mNext])
			{
				return NULL;
			}

			return next;
		}

		INLINE length_t getNodeSize(void* node) const
		{
			return reinterpret_cast<Node*>(static_cast<byte*>(node) - SIZEOF_HEAD)->size;
		}

		INLINE length_t getUsed() const
		{
			return mNext;
		}

		INLINE length_t getSize() const
		{
			return mSize;
		}

		INLINE void clear()
		{
			mNext = 0;
		}

		INLINE bool contains(void* object)
		{
			return object >= mHead && object < &mHead[mNext];
		}

		NOINLINE static void swap(SimpleMemoryAllocator& m1, SimpleMemoryAllocator& m2)
		{
			std::swap(m1.mHead, m2.mHead);
			std::swap(m1.mSize, m2.mSize);
			std::swap(m1.mNext, m2.mNext);
			std::swap(m1.mHeap, m2.mHeap);
		}
	};
};