#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "GrowableMemoryAllocator.h"


namespace Beer
{
	class BytecodeOutputStream
	{
	protected:
		byte* mData;
		//uint16 mDataLength;
		uint32 mDataCounter;

		uint32* mDict;
		uint16 mDictLength;
		uint16 mDictCounter;

		bool alignMemory;

		static GrowableMemoryAllocator gMemoryAllocator;
		static void* gMemory;

	public:
		NOINLINE BytecodeOutputStream(uint16 dictLength)
			: mData(NULL), mDictLength(dictLength), mDataCounter(0), mDict(NULL), mDictCounter(0), alignMemory(false)
		{
			if(!gMemory)
			{
				gMemory = gMemoryAllocator.malloc(dictLength * sizeof(uint16), 1024*1024); // 1MB
			}

			// allocate dictionary
			mDict = reinterpret_cast<uint32*>(gMemory);
			byte* ptr = reinterpret_cast<byte*>(gMemory) + (dictLength * sizeof(uint32));

			// clear dictionary
			memset(mDict, 0, mDictLength * sizeof(uint32));

			// allocate buffer for data - should be *AFTER* dictionary, so that it can grow
			mData = reinterpret_cast<byte*>(ptr);
		}

		INLINE ~BytecodeOutputStream()
		{
			// nothing
		}

		INLINE void setAlignMemory(bool value)
		{
			alignMemory = value;
		}

		INLINE void next()
		{
			if(alignMemory)
			{
				//mDataCounter += (mDataCounter % 4);
			}

			//DBG_ASSERT(mInstri < mDictSize, BEER_WIDEN("Instruction counter out of range"));
			mDict[mDictCounter++] = mDataCounter;
		}

		template <typename T>
		INLINE void write(const T& t)
		{
			//enlarge(sizeof(T));
			*reinterpret_cast<T*>(&mData[mDataCounter]) = t;
			move(sizeof(T));
		}

		INLINE void* alloc(uint16 length)
		{
			void* ptr = &mData[mDataCounter];
			move(length);
			return ptr;
		}

		INLINE void move(uint32 length)
		{
			mDataCounter += length;
		}

		INLINE void enlarge(uint32 size)
		{
			//mNewData.resize(mNewDataIndex + size); //old way
			// TODO: check if can memory grow that much
		}

		NOINLINE void finish(byte** data, uint32& dataLength, uint32** dict, uint16& dictLength)
		{
			// save dict
			*dict = mDict;
			dictLength = mDictLength;
			gMemory = reinterpret_cast<byte*>(gMemory) + (mDictLength * sizeof(uint32)); // permanenty allocate dictionary

			// save data
			*data = mData;
			dataLength = mDataCounter;
			gMemory = reinterpret_cast<byte*>(gMemory) + (mDataCounter * sizeof(byte)); // permanenty allocate data
		}
	};
};