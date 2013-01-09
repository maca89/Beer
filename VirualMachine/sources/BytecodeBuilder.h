#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "GrowableMemoryAllocator.h"


namespace Beer
{
	class BytecodeBuilder
	{
	protected:
		const byte* mOldData;
		uint32 mOldDataSize;
		uint32 mOldDataIndex;

		byte* mNewData;
		uint32 mNewDataIndex;

		uint16* mDict;
		uint32 mDictSize;
	
		uint32 mInstri;

		const Bytecode::Instruction* mOldInstr;
		Bytecode::Instruction* mNewInstr;

		static GrowableMemoryAllocator gMemoryAllocator;
		static void* gMemory;

	public:
		NOINLINE BytecodeBuilder(const byte* data, uint32 dataSize, uint32 dictSize)
			: mOldData(data), mOldDataSize(dataSize), mNewData(NULL), mDictSize(dictSize), mDict(NULL),
			mOldInstr(NULL), mNewInstr(NULL)
		{
			if(!gMemory)
			{
				gMemory = gMemoryAllocator.malloc(mDictSize * sizeof(uint16), 1024*1024); // 1MB
			}

			// allocate dictionary
			mDict = reinterpret_cast<uint16*>(gMemory);
			byte* toBeGMemory = reinterpret_cast<byte*>(gMemory) + (mDictSize * sizeof(uint16));

			// allocate buffer for data - should be *AFTER* dictionary, so that it can grow
			mNewData = reinterpret_cast<byte*>(toBeGMemory);

			// clear dictionary
			memset(mDict, 0, mDictSize * sizeof(uint16));

			mInstri = 0;
			mNewDataIndex = 0;
			mOldDataIndex = 0;
		}

		INLINE ~BytecodeBuilder()
		{
			// nothing
		}

		INLINE bool done()
		{
			return mOldDataIndex >= mOldDataSize;
		}

		INLINE void copy(uint16 size)
		{
			enlarge(size);
			for(uint16 i = 0; i < size; i++)
			{
				mNewData[mNewDataIndex++] = mOldData[mOldDataIndex++];
			}
		}

		INLINE void* alloc(uint16 size)
		{
			enlarge(size + 1);
			void* ptr = &mNewData[mNewDataIndex];
			mNewDataIndex += size;
			return ptr;
		}

		INLINE void enlarge(uint16 size)
		{
			//mNewData.resize(mNewDataIndex + size); //old way
			// TODO: check if can memory grow that much
		}

		NOINLINE Bytecode::OpCode/*Bytecode::Instruction**/ next()
		{
			DBG_ASSERT(mInstri < mDictSize, BEER_WIDEN("Instruction counter out of range"));
			mDict[mInstri++] = mNewDataIndex;
		
			copy(sizeof(uint8)); // copy opcode

			mOldInstr = reinterpret_cast<const Bytecode::Instruction*>(&mOldData[mOldDataIndex - sizeof(uint8)]);
			mNewInstr = reinterpret_cast<Bytecode::Instruction*>(&mNewData[mNewDataIndex - sizeof(uint8)]);

			return getOldOpcode();
		}

		INLINE void finish(byte** data, uint32& dataSize, uint16** dict, uint16& dictSize)
		{
			// save data
			*data = mNewData;
			dataSize = mNewDataIndex;
			gMemory = reinterpret_cast<byte*>(gMemory) + (mNewDataIndex * sizeof(byte)); // permanenty allocate data

			// save dict
			*dict = mDict;
			dictSize = mDictSize;
			gMemory = reinterpret_cast<byte*>(gMemory) + (mDictSize * sizeof(uint16)); // permanenty allocate dictionary

			// clear
			mDict = NULL;
			mNewData = NULL;
		}

		// opcode

		INLINE Bytecode::OpCode getOldOpcode() { return static_cast<Bytecode::OpCode>(mOldInstr->opcode); }
		INLINE Bytecode::OpCode getNewOpcode() { return static_cast<Bytecode::OpCode>(mNewInstr->opcode); }

		INLINE void setNewOpcode(Bytecode::OpCode value) { mNewInstr->opcode = value; }

		// get data
	
		INLINE uint64 getData_uint64() { return mOldInstr->getData_uint64(); }
		INLINE int64 getData_int64() { return mOldInstr->getData_int64(); }

		INLINE uint32 getData_uint32() { return mOldInstr->getData_uint32(); }
		INLINE int32 getData_int32() { return mOldInstr->getData_int32(); }

		INLINE uint16 getData_uint16() { return mOldInstr->getData_uint16(); }
		INLINE int16 getData_int16() { return mOldInstr->getData_int16(); }

		INLINE uint8 getData_uint8() { return mOldInstr->getData_uint8(); }
		INLINE int8 getData_int8() { return mOldInstr->getData_int8(); }

		// set data
	
		INLINE void setData_uint64(uint64 value) { alloc(sizeof(uint64)); mOldDataIndex += sizeof(uint64); mNewInstr->setData_uint64(value); }
		INLINE void setData_int64(int64 value) { alloc(sizeof(int64)); mOldDataIndex += sizeof(int64); mNewInstr->setData_int64(value); }
	
		INLINE void setData_uint32(uint32 value) { alloc(sizeof(uint32)); mOldDataIndex += sizeof(uint32); mNewInstr->setData_uint32(value); }
		INLINE void setData_int32(int32 value) { alloc(sizeof(int32)); mOldDataIndex += sizeof(int32); mNewInstr->setData_int32(value); }
	
		INLINE void setData_uint16(uint16 value) { alloc(sizeof(uint16)); mOldDataIndex += sizeof(uint16); mNewInstr->setData_uint16(value); }
		INLINE void setData_int16(int16 value) { alloc(sizeof(int16)); mOldDataIndex += sizeof(int16); mNewInstr->setData_int16(value); }
	
		INLINE void setData_uint8(uint8 value) { alloc(sizeof(uint8)); mOldDataIndex += sizeof(uint8); mNewInstr->setData_uint8(value); }
		INLINE void setData_int8(int8 value) { alloc(sizeof(int8)); mOldDataIndex += sizeof(int8); mNewInstr->setData_int8(value); }

	};
};