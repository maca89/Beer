#pragma once
#include "prereq.h"
#include "Bytecode.h"

//#define BEER_INLINE_FUNCTION_TABLE_CHECK(name2, opcode) if(strcmp(name, name2) == 0) return opcode;


namespace Beer
{
	class InlineFunctionTable
	{
	public:
		struct Pair
		{
			char* name;
			Bytecode::OpCode opcode;
		};

		typedef std::list<Pair> OpCodeMap;

	protected:
		OpCodeMap mOpCodes;
		char mBuff[1024];
		uint32 mBuffIndex;

	public:
		NOINLINE InlineFunctionTable()
		{
			mBuffIndex = 0;
			//memset(mBuff, 0, 1024);
		}

		NOINLINE ~InlineFunctionTable()
		{
		}

		INLINE void add(const char* name, Bytecode::OpCode opcode)
		{
			Pair pair;
			pair.name = &mBuff[mBuffIndex];
			size_t length = strlen(name) + 1;
			strcpy_s(pair.name, sizeof(char) * length, name);
			mBuffIndex += length;
			DBG_ASSERT(mBuffIndex < 1024, "InlineFunctionTable buffer overflow");
			pair.opcode = opcode;
			mOpCodes.push_back(pair);
		}

		NOINLINE void fill()
		{
			// boolean
			add("Boolean::==(Boolean)", Bytecode::INLINE_BOOLEAN_EQUAL);
			add("Boolean::!=(Boolean)", Bytecode::INLINE_BOOLEAN_NOT_EQUAL);
			add("Boolean::||(Boolean)", Bytecode::INLINE_BOOLEAN_OR);
			add("Boolean::&&(Boolean)", Bytecode::INLINE_BOOLEAN_AND);
			//integer
			add("Integer::+(Integer)", Bytecode::INLINE_INTEGER_PLUS);
			add("Integer::-(Integer)", Bytecode::INLINE_INTEGER_MINUS);
			add("Integer::*(Integer)", Bytecode::INLINE_INTEGER_MUL);
			add("Integer::<(Integer)", Bytecode::INLINE_INTEGER_SMALLER);
			add("Integer::<=(Integer)", Bytecode::INLINE_INTEGER_SMALLER_EQUAL);
			add("Integer::>(Integer)", Bytecode::INLINE_INTEGER_GREATER);
			add("Integer::>=(Integer)", Bytecode::INLINE_INTEGER_GREATER_EQUAL);
		}

		INLINE Bytecode::OpCode find(const char* name)
		{
			for(OpCodeMap::iterator it = mOpCodes.begin(); it != mOpCodes.end(); it++)
			{
				if(strcmp(it->name, name) == 0)
				{
					return it->opcode;
				}
			}

			return Bytecode::INSTR_NOP;
		}
	};
};