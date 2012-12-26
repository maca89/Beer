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

		byte* mHead;
		length_t mSize;
		length_t mNext;

	public:
		INLINE SimpleMemoryAllocator() : mHead(NULL), mSize(0), mNext(0) {}
		INLINE SimpleMemoryAllocator(length_t length) : mHead(0), mSize(0), mNext(0) { init(length); }
		INLINE ~SimpleMemoryAllocator() { SMART_DELETE(mHead); }

		INLINE void init(length_t size)
		{
			SMART_DELETE(mHead);
			mSize = size;
			mHead = new byte[mSize];
			memset(mHead, 0, mSize);
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

		INLINE void* getHead() const
		{
			return mHead + SIZEOF_HEAD;
		}

		INLINE void* getNext(void* object) const
		{
			byte* next = static_cast<byte*>(object) + getSize(object) + SIZEOF_HEAD;

			if(next >= &mHead[mNext])
			{
				return NULL;
			}

			return next;
		}

		INLINE length_t getSize(void* object) const
		{
			Node* node = reinterpret_cast<Node*>(static_cast<byte*>(object) - SIZEOF_HEAD);
			return node->size;
		}

		INLINE length_t getUsed() const
		{
			return mNext;
		}

		INLINE length_t getTotal() const
		{
			return mSize;
		}

		INLINE void reset()
		{
			mNext = 0;
		}

		INLINE void clear(uint32 defaultValue = 0)
		{
			reset();
			memset(mHead, defaultValue, mSize);
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
		}
	};
};