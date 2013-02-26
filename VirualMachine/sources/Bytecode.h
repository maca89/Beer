#pragma once
#include "prereq.h"
#include "StackFrame.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;
	class ClassFileDescriptor;
	class Method;
	class Thread;

	class Bytecode //: public Object, TODO
	{
	public:
		enum OpCode
		{
			// 0-9: internal
			INSTR_NOP				= 0,
			INSTR_JMP				= 1,
			INSTR_JMP_TRUE			= 2,
			INSTR_JMP_FALSE			= 3,
			// INSTR_THROW			= 4,
			// INSTR_TRY_BEGIN		= 5,
			// INSTR_TRY_END		= 6,
		
			// 10-19: stack control
			INSTR_POP				= 10,
			INSTR_TOP				= 11,
			INSTR_STORE				= 12,
			INSTR_MOVE_TOP			= 13,

			// 20-39: built-in types control
			INSTR_PUSH_INT8			= 20,
			//INSTR_PUSH_INT16		= 21,
			INSTR_PUSH_INT32		= 22,
			INSTR_PUSH_INT64		= 23,
			INSTR_PUSH_FLOAT		= 24,
			INSTR_PUSH_STRING		= 25,
			INSTR_PUSH_CHAR			= 26,
			INSTR_PUSH_BOOL			= 27,
			//INSTR_PUSH_STRUCT		= 28,
			//INSTR_PUSH_FUNC		= 29,

			// 40: object control
			INSTR_NEW				= 40,
			//INSTR_DELETE			= 41,
			INSTR_CLONE				= 42,
			INSTR_ASSIGN			= 43,
			INSTR_LOAD				= 44,
			INSTR_LOAD_THIS			= 45,// TODO

			// 50: method control
			INSTR_INVOKE			= 50,
			INSTR_INTERFACEINVOKE	= 51,
			INSTR_STATIC_INVOKE		= 52,
			INSTR_SPECIALINVOKE		= 53,
			INSTR_RETURN			= 55,
			
			_INSTR_SIZE				= 127,

			// inlined functions - the numbers may change!
			// Boolean
			INLINE_BOOLEAN_EQUAL = _INSTR_SIZE + 1,
			INLINE_BOOLEAN_NOT_EQUAL,
			INLINE_BOOLEAN_OR,
			INLINE_BOOLEAN_AND,
			INLINE_BOOLEAN_NEGATION,
			// Integer
			INLINE_INTEGER_PLUS,
			INLINE_INTEGER_MINUS,
			INLINE_INTEGER_MUL,
			INLINE_INTEGER_EQUAL,
			INLINE_INTEGER_NOT_EQUAL,
			INLINE_INTEGER_SMALLER,
			INLINE_INTEGER_SMALLER_EQUAL,
			INLINE_INTEGER_GREATER,
			INLINE_INTEGER_GREATER_EQUAL,
			// Array
			INLINE_ARRAY_GET_LENGTH,
			INLINE_ARRAY_GET_ITEM,
			INLINE_ARRAY_SET_ITEM,

			// optimalised instructions - the numbers may change!
			// Integer
			OPTIMAL_INTEGER_PUSH_INLINED,
			OPTIMAL_ARRAY_ALLOC,
		};

		#pragma pack(push, 1)
		struct Instruction
		{
			uint8 opcode;
			byte data; // should be used as array!

			// opcode

			INLINE OpCode getOpcode() const { return static_cast<OpCode>(opcode); }
			INLINE void setOpcode(OpCode value) { opcode = value; }

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
			NOINLINE void printTranslated(VirtualMachine* vm) const;
		};
		#pragma pack(pop)

	protected:
		uint16 mDictSize;
		uint16* mDict;
		
		uint32 mDataSize;
		byte* mData;

		uint8 mMethodCachesLength;
		

	public:
		INLINE Bytecode(byte data[], uint32 size, uint16 instrCount)
			: mDict(NULL), mDictSize(0), mMethodCachesLength(3)
		{
			mDataSize = size;
			mData = data;
			mDictSize = instrCount;
		}

		INLINE ~Bytecode()
		{
			// *NO* deleting of mData or mDict !!!
		}

		Method* call(Thread* thread, StackFrame* frame);
		void build(VirtualMachine* vm, ClassFileDescriptor* classFile);

		INLINE const Instruction* getInstruction(uint16 instri) const { return reinterpret_cast<const Instruction*>(&mData[mDict[instri]]); }
		INLINE Instruction* getInstruction(uint16 instri) { return reinterpret_cast<Instruction*>(&mData[mDict[instri]]); }

		INLINE uint16 getInstructionCount() const { return mDictSize; }

	protected:
		//INLINE bool isBuilt() const { return mDict != NULL; }
	};
};