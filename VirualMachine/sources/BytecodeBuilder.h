#pragma once
#include "prereq.h"
#include "Bytecode.h"


namespace Beer
{
	class BytecodeBuilder
	{
	protected:
		const byte* mOldData;
		uint32 mOldDataSize;
		uint32 mOldDataIndex;

		std::vector<byte> mNewData;
		uint32 mNewDataIndex;

		uint16* mDict;
		uint32 mDictSize;
	
		uint32 mInstri;

		const Bytecode::Instruction* mOldInstr;
		Bytecode::Instruction* mNewInstr;

	public:
		INLINE BytecodeBuilder(const byte* data, uint32 dataSize, uint32 dictSize)
			: mOldData(data), mOldDataSize(dataSize), mNewData((size_t)((double)dataSize * 10)), mDictSize(dictSize), mDict(NULL),
			mOldInstr(NULL), mNewInstr(NULL)
		{
			mDict = new uint16[mDictSize];
			memset(mDict, 0, mDictSize * sizeof(uint16));
			mInstri = 0;
			mNewDataIndex = 0;
			mOldDataIndex = 0;
		}

		INLINE ~BytecodeBuilder()
		{
			SMART_DELETE_ARR(mDict);
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
			mNewData.resize(mNewDataIndex + size); // allocate
		}

		NOINLINE Bytecode::OpCode/*Bytecode::Instruction**/ next()
		{
			DBG_ASSERT(mInstri < mDictSize, "Instruction counter out of range");
			mDict[mInstri++] = mNewDataIndex;
		
			copy(sizeof(uint8)); // copy opcode

			mOldInstr = reinterpret_cast<const Bytecode::Instruction*>(&mOldData[mOldDataIndex - sizeof(uint8)]);
			mNewInstr = reinterpret_cast<Bytecode::Instruction*>(&mNewData[mNewDataIndex - sizeof(uint8)]);

			return getOldOpcode();
		}

		NOINLINE void finish(byte** data, uint32& dataSize, uint16** dict, uint16& dictSize)
		{
			// alloc data
			*data = new byte[mNewDataIndex];
			memcpy(*data, &mNewData[0], mNewDataIndex * sizeof(byte));

			// alloc dict
			*dict = mDict;
			dictSize = mDictSize;

			mDict = NULL;
		}

		NOINLINE void finish(byte** data, uint32& dataSize)
		{
			// alloc data
			*data = new byte[mNewDataIndex];
			memcpy(*data, &mNewData[0], mNewDataIndex * sizeof(byte));
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