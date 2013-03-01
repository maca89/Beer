#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "StringClass.h"

//#define BEER_INLINE_FUNCTION_TABLE_CHECK(name2, opcode) if(strcmp(name, name2) == 0) return opcode;


namespace Beer
{
	class InlineFunctionTable
	{
	public:
		struct Pair
		{
			char_t* name;
			Bytecode::OpCode opcode;
		};

		typedef std::list<Pair> OpCodeMap;

	protected:
		OpCodeMap mOpCodes;
		char_t mBuff[1024];
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

		INLINE void add(const char_t* name, Bytecode::OpCode opcode)
		{
			Pair pair;
			pair.name = &mBuff[mBuffIndex];
			size_t length = strlen(name) + 1;
			strcpy(pair.name, sizeof(char_t) * length, name);
			mBuffIndex += length;
			DBG_ASSERT(mBuffIndex < 1024, BEER_WIDEN("InlineFunctionTable buffer overflow"));
			pair.opcode = opcode;
			mOpCodes.push_back(pair);
		}

		NOINLINE void fill()
		{
			// boolean
			add(BEER_WIDEN("Boolean::==(Boolean)"), BEER_INLINE_BOOLEAN_EQUAL);
			add(BEER_WIDEN("Boolean::!=(Boolean)"), BEER_INLINE_BOOLEAN_NOT_EQUAL);
			add(BEER_WIDEN("Boolean::||(Boolean)"), BEER_INLINE_BOOLEAN_OR);
			add(BEER_WIDEN("Boolean::&&(Boolean)"), BEER_INLINE_BOOLEAN_AND);
			add(BEER_WIDEN("Boolean::!()"), BEER_INLINE_BOOLEAN_NEGATION);
			// integer
			add(BEER_WIDEN("Integer::+(Integer)"), BEER_INLINE_INTEGER_PLUS);
			add(BEER_WIDEN("Integer::-(Integer)"), BEER_INLINE_INTEGER_MINUS);
			add(BEER_WIDEN("Integer::*(Integer)"), BEER_INLINE_INTEGER_MUL);
			add(BEER_WIDEN("Integer::<(Integer)"), BEER_INLINE_INTEGER_SMALLER);
			add(BEER_WIDEN("Integer::<=(Integer)"), BEER_INLINE_INTEGER_SMALLER_EQUAL);
			add(BEER_WIDEN("Integer::>(Integer)"), BEER_INLINE_INTEGER_GREATER);
			add(BEER_WIDEN("Integer::>=(Integer)"), BEER_INLINE_INTEGER_GREATER_EQUAL);
			add(BEER_WIDEN("Integer::==(Integer)"), BEER_INLINE_INTEGER_EQUAL);
			add(BEER_WIDEN("Integer::!=(Integer)"), BEER_INLINE_INTEGER_NOT_EQUAL);
			// array
			add(BEER_WIDEN("Array::getLength()"), BEER_INLINE_ARRAY_GET_LENGTH);
			add(BEER_WIDEN("Array::set(Integer,Integer)"), BEER_INLINE_ARRAY_SET_ITEM);
			add(BEER_WIDEN("Array::get(Integer)"), BEER_INLINE_ARRAY_GET_ITEM);
		}

		INLINE Bytecode::OpCode find(String* name)
		{
			return find(name->c_str());
		}

		INLINE Bytecode::OpCode find(const char_t* name)
		{
			for(OpCodeMap::iterator it = mOpCodes.begin(); it != mOpCodes.end(); it++)
			{
				if(strcmp(it->name, name) == 0)
				{
					return it->opcode;
				}
			}

			return BEER_INSTR_NOP;
		}
	};
};