#pragma once
#include "prereq.h"
#include "Frame.h"
#include "Pool.h"


namespace Beer
{
	class VirtualMachine;
	class Frame;
	class ClassFileDescriptor;
	class Method;
	class Thread;
	class Pool;

	class Bytecode //: public Object
	{
	public:
		enum 
		{
			// 0-9: internal
			#define BEER_INSTR_NOP				0
			#define BEER_INSTR_JMP				1
			#define BEER_INSTR_JMP_TRUE			2
			#define BEER_INSTR_JMP_FALSE		3
			// #define BEER_INSTR_THROW			4
			// #define BEER_INSTR_TRY_BEGIN		5
			// #define BEER_INSTR_TRY_END		6
		
			// 10-19: stack control
			#define BEER_INSTR_POP				10
			#define BEER_INSTR_TOP				11
			#define BEER_INSTR_STORE			12
			#define BEER_INSTR_MOVE_TOP			13

			// 20-39: built-in types control
			#define BEER_INSTR_PUSH_INT8		20
			//#define BEER_INSTR_PUSH_INT16		21
			#define BEER_INSTR_PUSH_INT32		22
			#define BEER_INSTR_PUSH_INT64		23
			#define BEER_INSTR_PUSH_FLOAT		24
			#define BEER_INSTR_PUSH_STRING		25
			#define BEER_INSTR_PUSH_CHAR		26
			#define BEER_INSTR_PUSH_BOOL		27
			//#define BEER_INSTR_PUSH_STRUCT	28
			//#define BEER_INSTR_PUSH_FUNC		29

			// 40: object control
			#define BEER_INSTR_NEW				40
			//#define BEER_INSTR_DELETE			41
			#define BEER_INSTR_CLONE			42
			#define BEER_INSTR_ASSIGN			43
			#define BEER_INSTR_LOAD				44
			#define BEER_INSTR_LOAD_THIS		45

			// 50: method control
			#define BEER_INSTR_VIRTUAL_INVOKE	50
			#define BEER_INSTR_INTERFACE_INVOKE 51
			#define BEER_INSTR_SPECIAL_INVOKE	53
			#define BEER_INSTR_STACK_INVOKE		54
			#define BEER_INSTR_RETURN			55
			
			#define BEER_INSTR_SIZE				127

			// inlined functions - the numbers may change!
			// Boolean
			#define BEER_INLINE_BOOLEAN_EQUAL			(BEER_INSTR_SIZE + 1)
			#define BEER_INLINE_BOOLEAN_NOT_EQUAL		(BEER_INSTR_SIZE + 2)
			#define BEER_INLINE_BOOLEAN_OR				(BEER_INSTR_SIZE + 3)
			#define BEER_INLINE_BOOLEAN_AND				(BEER_INSTR_SIZE + 4)
			#define BEER_INLINE_BOOLEAN_NEGATION		(BEER_INSTR_SIZE + 5)
			// Integer
			#define BEER_INLINE_INTEGER_PLUS			(BEER_INSTR_SIZE + 6)
			#define BEER_INLINE_INTEGER_MINUS			(BEER_INSTR_SIZE + 7)
			#define BEER_INLINE_INTEGER_MUL				(BEER_INSTR_SIZE + 8)
			#define BEER_INLINE_INTEGER_EQUAL			(BEER_INSTR_SIZE + 9)
			#define BEER_INLINE_INTEGER_NOT_EQUAL		(BEER_INSTR_SIZE + 10)
			#define BEER_INLINE_INTEGER_SMALLER			(BEER_INSTR_SIZE + 11)
			#define BEER_INLINE_INTEGER_SMALLER_EQUAL	(BEER_INSTR_SIZE + 12)
			#define BEER_INLINE_INTEGER_GREATER			(BEER_INSTR_SIZE + 13)
			#define BEER_INLINE_INTEGER_GREATER_EQUAL	(BEER_INSTR_SIZE + 14)
			// Array
			#define BEER_INLINE_ARRAY_GET_LENGTH		(BEER_INSTR_SIZE + 15)
			//#define BEER_INLINE_ARRAY_GET_ITEM			(BEER_INSTR_SIZE + 16)
			//#define BEER_INLINE_ARRAY_SET_ITEM			(BEER_INSTR_SIZE + 17)

			// optimalised instructions - the numbers may change!
			// Integer
			#define BEER_OPTIMAL_ARRAY_ALLOC			(BEER_INSTR_SIZE + 18)
			// Class
			#define BEER_OPTIMAL_CACHED_INVOKE			(BEER_INSTR_SIZE + 19)
			
			// important
			#define BEER_FILL_OPCODE_TABLE				(BEER_OPTIMAL_CACHED_INVOKE + 1)
			#define BEER_MAX_OPCODE						BEER_FILL_OPCODE_TABLE
		};

		
		typedef uint8 OpCode;

		#pragma pack(push, 1)
		struct Instruction // deprecated
		{
			uint8 opcode;
			byte data; // should be used as array!

			// opcode

			INLINE uint8 getOpcode() const { return static_cast<uint8>(opcode); }
			INLINE void setOpcode(uint8 value) { opcode = value; }

			// data

			INLINE const void* getData() const { return &data; }

			INLINE void* getData() { return &data; }

			template <typename T>
			INLINE const T getData() const { return *reinterpret_cast<const T*>(getData()); }

			template <typename T>
			INLINE T& getData() { return *reinterpret_cast<T*>(getData()); }

			template <typename T>
			INLINE void setData(T value) { *reinterpret_cast<T*>(getData()) = value; }

			// print

			NOINLINE uint16 printRaw(const ClassFileDescriptor* classFile) const; // returns size of the whole instruction
		};
		#pragma pack(pop)

	protected:
		uint16 mDictSize;
		uint32* mDict;

		uint32 mDataSize;
		byte* mData;

		uint8 mMethodCachesLength;

		Pool* mPool;

		static void* LabelTable[BEER_MAX_OPCODE * sizeof(void*)];
		

	public:
		INLINE Bytecode(byte data[], uint32 size, uint16 instrCount)
			: mDict(NULL), mDictSize(0), mMethodCachesLength(3), mPool(NULL)
		{
			mDataSize = size;
			mData = data;
			mDictSize = instrCount;
		}

		INLINE ~Bytecode()
		{
			// *NO* deleting of mData or mDict !!!
		}

		void call(Thread* thread);
		void build(Thread* thread, Method* method, ClassFileDescriptor* classFile);

		INLINE const Instruction* getInstruction(uint16 instri) const { return reinterpret_cast<const Instruction*>(&mData[mDict[instri]]); }
		INLINE Instruction* getInstruction(uint16 instri) { return reinterpret_cast<Instruction*>(&mData[mDict[instri]]); }

		INLINE uint16 getInstructionCount() const { return mDictSize; }

		void printTranslatedInstruction(Thread* thread, const Instruction* instr);

		static void init(Thread* thread); // important!!!

	protected:
		void checkPool(Thread* thread);
		uint16 createPoolSlot(Thread* thread);

		INLINE void loadFromPool(Thread* thread, uint16 index, StackRef<Object> ret)
		{
			Pool::getItem(thread, mPool, index, ret);
		}
		
		uint16 storeToPool(Thread* thread, StackRef<Object> object);
		
		INLINE void updateAtPool(Thread* thread, uint16 index, StackRef<Object> object)
		{
			Pool::setItem(thread, mPool, index, object);
		}

		void createPool(Thread* thread, uint16 length);
		//PolymorphicCache* createPolymorphicCache(Thread* thread, uint16 length);
	};
};