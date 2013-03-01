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

		NOINLINE void copy(uint16 size)
		{
			enlarge(size);
			for(uint16 i = 0; i < size; i++)
			{
				mNewData[mNewDataIndex++] = mOldData[mOldDataIndex++];
			}
		}

		NOINLINE void* alloc(uint16 size)
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

		NOINLINE Bytecode::OpCode next()
		{
			DBG_ASSERT(mInstri < mDictSize, BEER_WIDEN("Instruction counter out of range"));
			mDict[mInstri++] = mNewDataIndex;
		
			//copy(sizeof(uint8)); // copy opcode

			mOldInstr = reinterpret_cast<const Bytecode::Instruction*>(&mOldData[mOldDataIndex]);
			mNewInstr = reinterpret_cast<Bytecode::Instruction*>(&mNewData[mNewDataIndex]);

			mOldDataIndex += sizeof(uint8); // omit old opcode

			return mOldInstr->opcode;
		}

		template <typename T>
		INLINE void add(T value)
		{
			enlarge(sizeof(T));
			*((T*)&mNewData[mNewDataIndex]) = value;
			mNewDataIndex += sizeof(T);
		}

		NOINLINE void finish(byte** data, uint32& dataSize, uint16** dict, uint16& dictSize)
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

		//INLINE Bytecode::OpCode getOldOpcode() { return static_cast<Bytecode::OpCode>(mOldInstr->opcode); }

		INLINE const Bytecode::Instruction* getOldInstruction() const { return mOldInstr; }

		//INLINE Bytecode::OpCode getNewOpcode() { return static_cast<Bytecode::OpCode>(mNewInstr->opcode); }

		//INLINE void setNewOpcode(Bytecode::OpCode value) { mNewInstr->opcode = value; }

		// data

		template <typename T>
		INLINE const T getData()
		{
			mOldDataIndex += sizeof(T);
			return mOldInstr->getData<T>();
		}

		/*template <typename T>
		NOINLINE void setData(T value)
		{
			alloc(sizeof(T));
			mOldDataIndex += sizeof(T);
			mNewInstr->setData<T>(value);
		}*/
	};
};