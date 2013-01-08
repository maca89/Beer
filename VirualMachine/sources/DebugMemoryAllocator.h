#pragma once
#include "prereq.h"


namespace Beer
{
	class DebugMemoryAllocator
	{
	public:
		typedef uint32 length_t;
		//typedef uint64 length_t;
		enum { SIZEOF_HEAD = sizeof(length_t) };

		static const int DbgHeadValue = 0xfefefefe;
		static const int DbgDataValue = 0xcdcdcdcd;
		static const int DbgTailValue = 0xfdfdfdfd;

		static const int DbgHeadProtect = PAGE_READONLY;
		static const int DbgTailProtect = PAGE_NOACCESS;
		static const int DbgDataProtect = PAGE_READWRITE;

	protected:
		/*#pragma pack(push, 1)
		struct Node
		{
			length_t size;
			byte* ptr;
		};
		#pragma pack(pop)*/

		#pragma pack(push, 1)
		struct Block
		{
			length_t nodeSize;
			length_t blockSize;
		};
		#pragma pack(pop)


		byte* mHead;
		length_t mSize;
		length_t mNext;
		length_t mPageSize;

	public:
		INLINE DebugMemoryAllocator() : mHead(NULL), mSize(0), mNext(0), mPageSize(0) {}
		INLINE DebugMemoryAllocator(length_t length) : mHead(NULL), mSize(0), mNext(0), mPageSize(0) { init(length); }
		INLINE ~DebugMemoryAllocator() { destroy(); }

		INLINE void init(length_t length)
		{
			SYSTEM_INFO si;
			::GetSystemInfo(&si);
			mPageSize = si.dwPageSize;

			mSize = length;
			mHead = static_cast<byte*>(VirtualAlloc(NULL, mSize, MEM_RESERVE, PAGE_NOACCESS));
			clear();
		}

		INLINE void destroy()
		{
			VirtualFree(mHead, mSize, MEM_RELEASE);
			mSize = 0;
		}

		INLINE void* malloc(length_t size)
		{
			length_t pages = howMuchPages(size) + 2; // +2 for guards
			length_t blockSize = pages * mPageSize;
			
			if(!canAllocMemory(blockSize))
			{
				return NULL;
			}

			Block* block = reinterpret_cast<Block*>(&mHead[mNext]);
			void* node = reinterpret_cast<byte*>(block) + mPageSize;
			void* tail = reinterpret_cast<byte*>(node) + (pages - 2) * mPageSize;

			DWORD oldProtect = 0;

			// page before data (head)
			VirtualAlloc(block, mPageSize, MEM_COMMIT, PAGE_READWRITE); // alloc & make temporary writable
			memset(block, DbgHeadValue, mPageSize); // set guard value
			block->nodeSize = size; // save data
			block->blockSize = blockSize; // save while block size
			VirtualProtect(block, mPageSize, DbgHeadProtect, &oldProtect); // protect page

			// page for data
			VirtualAlloc(node, (pages - 2) * mPageSize, MEM_COMMIT, PAGE_READWRITE); // alloc & make writable
			memset(node, DbgDataValue, (pages - 2) * mPageSize); // set guard value
			VirtualProtect(node, (pages - 2) * mPageSize, DbgDataProtect, &oldProtect); // protect page

			// page after data (tail)
			VirtualAlloc(tail, mPageSize, MEM_COMMIT, PAGE_READWRITE); // alloc & make temporary writable
			memset(tail, DbgTailValue, mPageSize); // set guard value
			VirtualProtect(tail, mPageSize, DbgTailProtect, &oldProtect); // protect page

			mNext += blockSize;

			return node;
		}

		INLINE bool canMalloc(length_t size) const
		{
			return canAllocMemory((howMuchPages(size) + 2) * mPageSize); // +2 for guards
		}

		INLINE void* getFirstNode() const
		{
			return mHead + mPageSize;
		}

		INLINE void* getNextNode(void* node) const
		{
			Block* block = getBlock(node);
			void* next = reinterpret_cast<byte*>(block) + block->blockSize;

			if(next >= &mHead[mNext])
			{
				return NULL;
			}

			return reinterpret_cast<byte*>(next) + mPageSize;
		}

		INLINE length_t getNodeSize(void* node) const
		{
			Block* block = getBlock(node);
			return block->nodeSize;
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
			/*DWORD oldProtect = 0;
			VirtualProtect(mHead, mSize, PAGE_READWRITE, &oldProtect); // so that you we can memset memory in the next statement
			memset(mHead, DbgDeletedValue, mSize);
			VirtualProtect(mHead, mSize, DbgDeletedProtect, &oldProtect);*/

			// TODO: traverse all blocks and check for memory write
			
			if(mNext > 0) VirtualFree(mHead, mNext, MEM_DECOMMIT);
			mNext = 0;
		}

		INLINE bool contains(void* object)
		{
			return object >= mHead && object < &mHead[mNext];
		}

		NOINLINE static void swap(DebugMemoryAllocator& m1, DebugMemoryAllocator& m2)
		{
			std::swap(m1.mHead, m2.mHead);
			std::swap(m1.mSize, m2.mSize);
			std::swap(m1.mNext, m2.mNext);
			std::swap(m1.mPageSize, m2.mPageSize);
		}

	protected:
		INLINE length_t howMuchPages(length_t memory) const
		{
			return (memory / mPageSize) + ((memory % mPageSize) == 0 ? 0 : 1);
		}

		INLINE bool canAllocMemory(length_t memory) const
		{
			return mNext + memory < mSize;
		}

		INLINE bool canAllocPages(length_t pagesCount) const
		{
			return canAllocMemory(pagesCount * mPageSize);
		}

		INLINE length_t getBlockSize(void* headPage) const
		{
			return reinterpret_cast<length_t*>(headPage)[1];
		}

		INLINE Block* getBlock(void* dataPage) const
		{
			return reinterpret_cast<Block*>(reinterpret_cast<byte*>(dataPage) - mPageSize);
		}
	};
};