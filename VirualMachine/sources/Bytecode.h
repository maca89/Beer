#pragma once
#include "prereq.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;
	class ClassFileDescriptor;

	class Bytecode
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

			// 50: method control
			INSTR_INVOKE			= 50,
			INSTR_INTERFACEINVOKE	= 51,
			INSTR_STATIC_INVOKE		= 52,
			INSTR_SPECIALINVOKE		= 53,
			INSTR_RETURN			= 55,
			
			_INSTR_SIZE			= 127,

			// 100: inlined functions - the numbers may be changed!
			// Boolean
			INLINE_BOOLEAN_EQUAL = 128,
			INLINE_BOOLEAN_NOT_EQUAL = 129,
			INLINE_BOOLEAN_OR = 130,
			INLINE_BOOLEAN_AND = 131,
			// Integer
			INLINE_INTEGER_PLUS = 132,
			INLINE_INTEGER_MINUS = 133,
			INLINE_INTEGER_MUL = 134,
			INLINE_INTEGER_EQUAL = 135,
			INLINE_INTEGER_NOT_EQUAL = 136,
			INLINE_INTEGER_SMALLER = 137,
			INLINE_INTEGER_SMALLER_EQUAL = 138,
			INLINE_INTEGER_GREATER = 139,
			INLINE_INTEGER_GREATER_EQUAL = 140,
		};

		#pragma pack(push, 1)
		struct Instruction
		{
			uint8 opcode;
			byte data; // should be used as array!
			
			// int 1 byte

			INLINE uint8 getData_uint8() const { return static_cast<uint8>(data); }
			INLINE int8 getData_int8() const { return static_cast<int8>(data); }

			INLINE uint8& getData_uint8() { return reinterpret_cast<uint8&>(data); }
			INLINE int8& getData_int8() { return reinterpret_cast<int8&>(data); }

			INLINE void setData_uint8(uint8 v) { getData_uint8() = v; }
			INLINE void setData_int8(int8 v) { getData_int8() = v; }
			
			// int 2 bytes

			INLINE uint16 getData_uint16() const { return *reinterpret_cast<const uint16*>(&data); }
			INLINE int16 getData_int16() const { return *reinterpret_cast<const int16*>(&data); }

			INLINE uint16& getData_uint16() { return *reinterpret_cast<uint16*>(&data); }
			INLINE int16& getData_int16() { return *reinterpret_cast<int16*>(&data); }

			INLINE void setData_uint16(uint16 v) { getData_uint16() = v; }
			INLINE void setData_int16(int16 v) { getData_int16() = v; }

			// int 4 bytes

			INLINE uint32 getData_uint32() const { return *reinterpret_cast<const uint32*>(&data); }
			INLINE int32 getData_int32() const { return *reinterpret_cast<const int32*>(&data); }

			INLINE uint32& getData_uint32() { return *reinterpret_cast<uint32*>(&data); }
			INLINE int32& getData_int32() { return *reinterpret_cast<int32*>(&data); }

			INLINE void setData_uint32(uint32 v) { getData_uint32() = v; }
			INLINE void setData_int32(int32 v) { getData_int32() = v; }

			// int 8 bytes

			INLINE uint64 getData_uint64() const { return *reinterpret_cast<const uint64*>(&data); }
			INLINE int64 getData_int64() const { return *reinterpret_cast<const int64*>(&data); }

			INLINE uint64& getData_uint64() { return *reinterpret_cast<uint64*>(&data); }
			INLINE int64& getData_int64() { return *reinterpret_cast<int64*>(&data); }

			INLINE void setData_uint64(uint64 v) { getData_uint64() = v; }
			INLINE void setData_int64(int64 v) { getData_int64() = v; }

			// float 4 bytes

			INLINE float32 getData_float32() const { return *reinterpret_cast<const float32*>(&data); }

			INLINE float32& getData_float32() { return *reinterpret_cast<float32*>(&data); }

			INLINE void setData_float32(float32 v) { getData_float32() = v; }

			// float 8 bytes

			INLINE float64 getData_float64() const { return *reinterpret_cast<const float64*>(&data); }

			INLINE float64& getData_float64() { return *reinterpret_cast<float64*>(&data); }

			INLINE void setData_float64(float64 v) { getData_float64() = v; }

			// print

			NOINLINE uint16 printRaw(const ClassFileDescriptor* classFile) const; // returns size of the whole instruction
			NOINLINE void printTranslated() const;
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
			SMART_DELETE_ARR(mDict);
		}

		void call(VirtualMachine* vm, StackFrame* frame);
		void build(VirtualMachine* vm, ClassFileDescriptor* classFile);

		INLINE const Instruction* getInstruction(uint16 instri) const { return reinterpret_cast<const Instruction*>(&mData[mDict[instri]]); }

	protected:
		//INLINE bool isBuilt() const { return mDict != NULL; }
	};
};