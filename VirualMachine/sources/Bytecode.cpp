#include "stdafx.h"
#include "Bytecode.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "String.h"
#include "Boolean.h"
#include "Integer.h"
#include "Float.h"
#include "Console.h"
#include "Method.h"
#include "BytecodeInputStream.h"
#include "BytecodeOutputStream.h"
#include "PolymorphicCache.h"
#include "Debugger.h"
#include "Array.h"
#include "GenerationalGC.h"
#include "StringDescriptor.h"
#include "ClassFileDescriptor.h"

#include "FrameInspector.h"

using namespace Beer;

#define BEER_BC_DISPATCH_SWITCH 0
#define BEER_BC_DISPATCH_DIRECT 1

#define BEER_BC_DISPATCH	BEER_BC_DISPATCH_SWITCH


void* Bytecode::LabelTable[BEER_MAX_OPCODE * sizeof(void*)] = {0};

#ifdef BEER_BC_DEBUGGING
	std::map<void*, Bytecode::OpCode> Bytecode::LabelTableDebugDictionary;
#endif // BEER_BC_DEBUGGING


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// common
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BEER_BC_PRINT() cout << *reinterpret_cast<BEER_BC_OPCODE_TYPE*>(ip) << "\t"; printTranslatedInstruction(thread, invokedMethod, ip); cout << "\n";

#define BEER_BC_RETURN() frame->ip(ip); return;

#define BEER_BC_MOVE(howmuch) ip += (howmuch);

#define BEER_BC_DATA(type) *reinterpret_cast<type*>(ip)

#define BEER_BC_OPCODE() BEER_BC_DATA(BEER_BC_OPCODE_TYPE)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH

#define BEER_BC_WORD int32

#define BEER_BC_UWORD uint32

#define BEER_BC_OPCODE_TYPE BEER_BC_UWORD

#define BEER_BC_WRITE_OPCODE(ostream, opcode) ostream.write<BEER_BC_OPCODE_TYPE>(opcode);

#define BEER_BC_JUMP() ip += BEER_BC_DATA(BEER_BC_WORD);

#define BEER_BC_NEXT(skip) BEER_BC_MOVE(skip); BEER_BC_PASS();

#define BEER_BC_JUMP_NEXT() BEER_BC_JUMP(); BEER_BC_NEXT_0();

#define BEER_BC_NEXT_0() BEER_BC_PASS();

#define BEER_BC_PASS() break;

#define BEER_BC_START while(true) { /*BEER_BC_PRINT();*/ BEER_BC_OPCODE_TYPE opcode = BEER_BC_OPCODE(); BEER_BC_MOVE(sizeof(BEER_BC_OPCODE_TYPE)); switch(opcode) {

#define BEER_BC_END }} BEER_BC_RETURN();

#define BEER_BC_LABEL(name) case BEER_##name

#define BEER_BC_DEFAULT_LABEL() default

#define BEER_BC_SIZEOF(t) sizeof(t)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT

#define BEER_BC_WORD int32

#define BEER_BC_UWORD uint32

#define BEER_BC_OPCODE_TYPE void*

#define BEER_BC_WRITE_OPCODE(ostream, opcode) ostream.write<BEER_BC_OPCODE_TYPE>((BEER_BC_OPCODE_TYPE)Bytecode::LabelTable[opcode]);
	
#define BEER_BC_JUMP() ip += BEER_BC_DATA(BEER_BC_WORD)

#define BEER_BC_JUMP_NEXT() BEER_BC_JUMP(); BEER_BC_NEXT_0();

// ip += BEER_BC_DATA(BEER_BC_WORD)
#define __BEER_BC_JUMP()\
	__asm mov ebx, dword ptr[ip]\
	__asm mov ecx, dword ptr[ip]\
	__asm add ebx, dword ptr[ecx]\
	__asm mov dword ptr[ip], ebx

// BEER_BC_MOVE(skip)
// BEER_BC_PASS()
#define BEER_BC_NEXT(skip)\
	/* void* eax = ip + skip */\
	__asm mov ebx, dword ptr[ip]\
	__asm add ebx, skip\
	/* void* ecx = *ebx */\
	__asm mov ecx, dword ptr[ebx]\
	/* ip = ebx + sizeof(BEER_BC_OPCODE_TYPE) */\
	__asm add ebx, 4\
	__asm mov dword ptr[ip], ebx\
	/* jump ecx */\
	__asm jmp ecx

// BEER_BC_PASS()
#define BEER_BC_NEXT_0()\
	BEER_BC_PASS()

// void* jumpAddr = *ip;
// ip += sizeof(BEER_BC_OPCODE_TYPE);
// jmp jumpAddr;
#define BEER_BC_PASS()\
	/* void* ebx = ip */\
	__asm mov ebx, dword ptr[ip]\
	/* void* ecx = *ebx */\
	__asm mov ecx, dword ptr[ebx]\
	/* ip = ebx + 4 */\
	__asm add ebx, 4\
	__asm mov dword ptr[ip], ebx\
	/* jump ecx */\
	__asm jmp ecx

#define BEER_BC_START BEER_BC_CHECK_TABLE_FILL(); BEER_BC_PASS();

#define BEER_BC_END BEER_BC_RETURN();

#define BEER_BC_LABEL(name) LABEL_##name

// TODO
#define BEER_BC_DEFAULT_LABEL() BEER_BC_LABEL(UNKNOWN_OPCODE)

#define BEER_BC_SIZEOF(t) SIZE t
	
// just a shortcut for BEER_STORE_ADDRESS with one argument
#define BEER_CALLABLE_LABEL(name) BEER_STORE_ADDRESS(BEER_##name, LABEL_##name)

#define BEER_STORE_ADDRESS(index, label)											\
{																					\
	void** data = Bytecode::LabelTable;												\
	__asm lea eax, label															\
	__asm mov edx, data																\
	__asm mov [edx][index * TYPE data], eax											\
}																					\

#define BEER_BC_CHECK_TABLE_FILL() if(*reinterpret_cast<BEER_BC_UWORD*>(ip) == BEER_FILL_OPCODE_TABLE) goto LABEL_FILL_OPCODE_TABLE;

#endif // BEER_BC_DISPATCH
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BEER_BC_CHECK_SAFEPOINT() if(thread->getVM()->isSafePoint()) { BEER_BC_MOVE(-static_cast<int64>(sizeof(BEER_BC_OPCODE_TYPE))); BEER_BC_RETURN(); }

uint16 Bytecode::Instruction::printRaw(const ClassFileDescriptor* classFile) const
{
	uint16 size = sizeof(uint8); // opcode

	switch (opcode)
	{
	case BEER_INSTR_NOP:
		cout << "NOP";
		break;

	case BEER_INSTR_JMP:
		cout << "JMP " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_POP:
		cout << "POP";
		break;

	case BEER_INSTR_TOP:
		cout << "TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_STORE:
		cout << "STORE " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData<int8>());
		size += sizeof(int8);
		break;

	case BEER_INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData<int32>();
		size += sizeof(int32);
		break;

	case BEER_INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData<int64>();
		size += sizeof(int64);
		break;

	case BEER_INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData<float64>();
		size += sizeof(float64);
		break;

	case BEER_INSTR_PUSH_STRING:
		cout << "PUSH_STRING " << getData<uint32>();
		cout << " // \"" << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str() << "\"";
		size += sizeof(uint32);
		break;

	case BEER_INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16();
		//size += sizeof(uint16);
		cout << "PUSH_CHAR " << getData<uint8>();
		size += sizeof(uint8);
		break;

	case BEER_INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData<uint8>());
		size += sizeof(uint8);
		break;

	case BEER_INSTR_NEW:
		cout << "NEW " << getData<uint32>();
		cout << " // " << classFile->getClassName(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_ASSIGN_THIS:
		cout << "ASSIGN_THIS " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_VIRTUAL_INVOKE:
		cout << "VIRTUAL_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_INTERFACE_INVOKE:
		cout << "INTERFACE_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_SPECIAL_INVOKE:
		cout << "SPECIAL_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_STACK_INVOKE:
		cout << "STACK_INVOKE";
		break;
	
	case BEER_INSTR_RETURN:
		cout << "RETURN " << getData<uint16>();
		size += sizeof(uint16);
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}

	return size;
}

void Bytecode::printTranslatedInstruction(Thread* thread, Method* method, byte* ip)
{
//#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH
	const BEER_BC_OPCODE_TYPE opcode = BEER_BC_OPCODE();

#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	const OpCode opcode = 
	#ifdef BEER_BC_DEBUGGING
		LabelTableDebugDictionary[BEER_BC_OPCODE()];
	#else
		0;
		cout << "*Available only in debug mode*";
		return;
	#endif // BEER_BC_DEBUGGING
#endif // BEER_BC_DISPATCH

	BEER_BC_MOVE(sizeof(BEER_BC_OPCODE_TYPE));

	switch (opcode)
	{
	case BEER_INSTR_NOP: cout << "NOP"; break;
	case BEER_INSTR_JMP: cout << "JMP " << BEER_BC_DATA(int32); break;
	case BEER_INSTR_JMP_TRUE: cout << "JMP_TRUE " << BEER_BC_DATA(int32); break;
	case BEER_INSTR_JMP_FALSE: cout << "JMP_FALSE " << BEER_BC_DATA(int32); break;
	case BEER_INSTR_POP: cout << "POP"; break;
	case BEER_INSTR_TOP: cout << "TOP " << BEER_BC_DATA(int16); break;
	case BEER_INSTR_STORE: cout << "STORE " << BEER_BC_DATA(int16); break;
	case BEER_INSTR_MOVE_TOP: cout << "MOVE_TOP " << BEER_BC_DATA(int16); break;

	case BEER_INSTR_PUSH_INT64:
		{
			Integer* value = static_cast<Integer*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "PUSH_INT64 " << value->getData();
		}
		break;

	case BEER_INSTR_PUSH_FLOAT:
		{
			Float* value = static_cast<Float*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "PUSH_FLOAT " << value->getData();
		}
		break;

	case BEER_INSTR_PUSH_STRING:
		{
			String* value = static_cast<String*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "PUSH_STRING \"" << value->c_str() << "\"";
		}
		break;

	case BEER_INSTR_PUSH_CHAR:
		{
			Character* value = static_cast<Character*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "PUSH_CHAR " << value->getData();
		}
		break;

	case BEER_INSTR_PUSH_BOOL:
		{
			Boolean* value = static_cast<Boolean*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "PUSH_BOOL " << value->getData();
		}
		break;

	case BEER_INSTR_PUSH_THIS: cout << "PUSH_THIS"; break;

	case BEER_INSTR_NEW: // ugly, TODO
		{
			Class* klass = static_cast<Class*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			String* name = klass->getName();
			cout << "NEW " << name->c_str();
		}
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << BEER_BC_DATA(uint16);
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << BEER_BC_DATA(uint16);
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << BEER_BC_DATA(uint16);
		break;

	case BEER_INSTR_ASSIGN_THIS:
		cout << "ASSIGN_THIS " << BEER_BC_DATA(uint16);
		break;
	
	case BEER_INSTR_INTERFACE_INVOKE:
		{
			StackRef<String> selector(frame, frame->stackPush());
			method->loadFromPool(thread, BEER_BC_DATA(uint16), selector);

			cout << "INTERFACE_INVOKE \"" << selector->c_str() << "\"";
			
			frame->stackPop(); // pop selector
		}
		break;

	case BEER_INSTR_VIRTUAL_INVOKE:
		{
			cout << "VIRTUAL_INVOKE #" << BEER_BC_DATA(uint32);
		}
		break;
	
	case BEER_INSTR_SPECIAL_INVOKE:
		{
			Method* method = static_cast<Method*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "SPECIAL_INVOKE " << method->getSelector()->c_str();
		}
		break;

	case BEER_OPTIMAL_CACHED_INVOKE:
		{
			Method* method = static_cast<Method*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
			cout << "CACHED_INVOKE " << method->getSelector()->c_str();
		}

	case BEER_INSTR_STACK_INVOKE: cout << "STACK_INVOKE"; break;
	case BEER_INSTR_RETURN: cout << "RETURN "; break;
	case BEER_INLINE_BOOLEAN_EQUAL: cout << "BOOLEAN_EQUAL"; break;
	case BEER_INLINE_BOOLEAN_NOT_EQUAL: cout << "BOOLEAN_NOT_EQUAL"; break;
	case BEER_INLINE_BOOLEAN_OR: cout << "BOOLEAN_OR"; break;
	case BEER_INLINE_BOOLEAN_AND: cout << "BOOLEAN_AND"; break;
	case BEER_INLINE_BOOLEAN_NEGATION: cout << "BOOLEAN_NEGATION"; break;
	case BEER_INLINE_INTEGER_PLUS: cout << "INTEGER_PLUS"; break;
	case BEER_INLINE_INTEGER_MINUS: cout << "INTEGER_MINUS"; break;
	case BEER_INLINE_INTEGER_MUL: cout << "INTEGER_MUL"; break;
	case BEER_INLINE_INTEGER_EQUAL: cout << "INTEGER_EQUAL"; break;
	case BEER_INLINE_INTEGER_NOT_EQUAL: cout << "INTEGER_NOT_EQUAL"; break;
	case BEER_INLINE_INTEGER_SMALLER: cout << "INTEGER_SMALLER"; break;
	case BEER_INLINE_INTEGER_SMALLER_EQUAL: cout << "INTEGER_SMALLER_EQUAL"; break;
	case BEER_INLINE_INTEGER_GREATER: cout << "INTEGER_GREATER"; break;
	case BEER_INLINE_INTEGER_GREATER_EQUAL: cout << "INTEGER_GREATER_EQUAL"; break;
	case BEER_INLINE_ARRAY_GET_LENGTH: cout << "ARRAY_GET_LENGTH"; break;
	case BEER_INLINE_ARRAY_GET_ITEM: cout << "ARRAY_GET"; break;
	case BEER_INLINE_ARRAY_SET_ITEM: cout << "ARRAY_SET"; break;
	case BEER_OPTIMAL_ARRAY_ALLOC: cout << "NEW_ARRAY"; break;
	case BEER_OPTIMAL_PUSH_TRUE: cout << "PUSH_TRUE"; break;
	case BEER_OPTIMAL_PUSH_FALSE: cout << "PUSH_FALSE"; break;

	default: cout << "[UNKNOWN]"; break;
	}
}

void DefaultBytecodeCompiler::compile(Thread* thread, Method* method, const TemporaryBytecode& bc, TemporaryBytecode& out_bc)
{
	BytecodeInputStream istream(bc.data, bc.dataLength);
	BytecodeOutputStream ostream(bc.instrCount);

	while(!istream.end())
	{
		ostream.next();
		Bytecode::OpCode opcode = istream.read<Bytecode::OpCode>();

		switch(opcode)
		{
			// no args
			case BEER_INSTR_NOP:
			case BEER_INSTR_POP:
			case BEER_FILL_OPCODE_TABLE:
			case BEER_INSTR_STACK_INVOKE:
			case BEER_INSTR_PUSH_THIS:
			// Boolean
			case BEER_INLINE_BOOLEAN_EQUAL:
			case BEER_INLINE_BOOLEAN_NOT_EQUAL:
			case BEER_INLINE_BOOLEAN_OR:
			case BEER_INLINE_BOOLEAN_AND:
			case BEER_INLINE_BOOLEAN_NEGATION:
			// Integer
			case BEER_INLINE_INTEGER_PLUS:
			case BEER_INLINE_INTEGER_MINUS:
			case BEER_INLINE_INTEGER_MUL:
			case BEER_INLINE_INTEGER_EQUAL:
			case BEER_INLINE_INTEGER_NOT_EQUAL:
			case BEER_INLINE_INTEGER_SMALLER:
			case BEER_INLINE_INTEGER_SMALLER_EQUAL:
			case BEER_INLINE_INTEGER_GREATER:
			case BEER_INLINE_INTEGER_GREATER_EQUAL:
			// Array
			case BEER_INLINE_ARRAY_GET_LENGTH:
			case BEER_INLINE_ARRAY_GET_ITEM:
			case BEER_INLINE_ARRAY_SET_ITEM:
			// optimalised instructions
			case BEER_OPTIMAL_ARRAY_ALLOC:
			case BEER_OPTIMAL_PUSH_TRUE:
			case BEER_OPTIMAL_PUSH_FALSE:
				BEER_BC_WRITE_OPCODE(ostream, opcode);
				break;

			// word arg (extended from int16)
			case BEER_INSTR_MOVE_TOP:
			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
			case BEER_INSTR_RETURN:
				{
					BEER_BC_WRITE_OPCODE(ostream, opcode);

					BEER_BC_WORD arg = istream.read<int16>();
					ostream.write<BEER_BC_WORD>(arg);
				}
				break;

			// unsigned word (extended from uint16)
			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_ASSIGN:
			case BEER_INSTR_ASSIGN_THIS:
				{
					BEER_BC_WRITE_OPCODE(ostream, opcode);

					BEER_BC_UWORD arg = istream.read<uint16>();
					ostream.write<BEER_BC_UWORD>(arg);
				}
				break;

			// uint32 arg (index)
			case BEER_INSTR_VIRTUAL_INVOKE:
				BEER_BC_WRITE_OPCODE(ostream, opcode);
				ostream.write<uint32>(istream.read<uint32>());
				break;

			// int32 arg (object pointer)
			case BEER_INSTR_PUSH_STRING:
			case BEER_INSTR_PUSH_BOOL:
			case BEER_INSTR_PUSH_CHAR:
			case BEER_INSTR_PUSH_FLOAT:
			case BEER_INSTR_PUSH_INT64:
			case BEER_OPTIMAL_CACHED_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				BEER_BC_WRITE_OPCODE(ostream, opcode);
				ostream.write<int32>(istream.read<int32>());
				break;
				
			// two uint16 args
			case BEER_INSTR_INTERFACE_INVOKE:
				BEER_BC_WRITE_OPCODE(ostream, opcode);
				ostream.write<uint16>(istream.read<uint16>()); // selector
				ostream.write<uint16>(istream.read<uint16>()); // cache
				break;

			// two int32 args (object pointer)
			case BEER_INSTR_NEW:
				{
				BEER_BC_WRITE_OPCODE(ostream, opcode);

				/*Class* klass = static_cast<Class*>(reinterpret_cast<Object*>(istream.read<int32>()));
				cout << klass->getName()->c_str() << "\n";*/
				
				//ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(klass)));
				ostream.write<int32>(istream.read<int32>());
				ostream.write<int32>(istream.read<int32>());
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}
	
	ostream.finish(&out_bc.data, out_bc.dataLength, &out_bc.dict, out_bc.instrCount);
	//method->setBytecode(new Bytecode(out_bc.data, out_bc.dataLength));

	// fix indices
	for(uint16 instri = 0; instri < out_bc.instrCount; instri++)
	{
		uint16 instrAddr = out_bc.dict[instri];
		byte* ip = &out_bc.data[instrAddr];
		BEER_BC_OPCODE_TYPE opcode = *reinterpret_cast<BEER_BC_OPCODE_TYPE*>(ip);

	#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH
		if(opcode == BEER_INSTR_JMP || opcode == BEER_INSTR_JMP_TRUE || opcode == BEER_INSTR_JMP_FALSE)
	#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
		if(opcode == Bytecode::LabelTable[BEER_INSTR_JMP] || opcode == Bytecode::LabelTable[BEER_INSTR_JMP_TRUE] || opcode == Bytecode::LabelTable[BEER_INSTR_JMP_FALSE])
	#endif // BEER_BC_DISPATCH
		{
			// absolute offset from current instr (number of bytes)
			BEER_BC_UWORD* arg = reinterpret_cast<BEER_BC_UWORD*>(&ip[sizeof(BEER_BC_OPCODE_TYPE)]);
			uint16 jumpAddress = out_bc.dict[*arg];

			// -sizeof(opcode) for the current opcode of jmp
			*reinterpret_cast<BEER_BC_WORD*>(arg) = static_cast<BEER_BC_WORD>(static_cast<int32>(jumpAddress) - static_cast<int32>(instrAddr)) - sizeof(BEER_BC_OPCODE_TYPE);

			// relative offset from current instr (number of instrs)
			/*int16 offset = static_cast<int16>(static_cast<int32>(instr->getData<uint16>()) - static_cast<int32>(instri));
			instr->setData<int16>(offset);*/
		}
	}
}

void Bytecode::invokeBytecode(Thread* thread)
{
	Frame* frame = thread->getFrame();

	Method* invokedMethod = frame->stackTop<Method>(-1);
	DBG_ASSERT(invokedMethod != NULL, BEER_WIDEN("Method is NULL"));

	byte* ip = reinterpret_cast<byte*>(frame->ip());
	//void* jumpAddr = NULL;

	if(ip == NULL)
	{
		ip = reinterpret_cast<byte*>(invokedMethod->getBytecode()->getData());
		DBG_ASSERT(ip != NULL, BEER_WIDEN("Null bytecode"));
	}

	// important for debugging
#ifdef BEER_DEBUG_MODE
	if(thread->getVM()->getDebugger()->isEnabled()) thread->getVM()->getDebugger()->step(thread, frame);
#endif // BEER_DEBUG_MODE
		
	//DBG_ASSERT(frame->programCounter < mDictSize, BEER_WIDEN("Program counter out of range"));
	//DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stackSize() - 1), BEER_WIDEN("Broken stack"));

	BEER_BC_START

BEER_BC_LABEL(INSTR_NOP):
	BEER_BC_NEXT_0();

BEER_BC_LABEL(INSTR_JMP):
	BEER_BC_JUMP_NEXT();

BEER_BC_LABEL(INSTR_JMP_TRUE):
	DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

	if(frame->stackTop<Boolean>()->getData())
	{
		BEER_BC_JUMP();
	}
	else
	{
		BEER_BC_MOVE(sizeof(BEER_BC_WORD));
	}

	frame->stackPop();
	BEER_BC_NEXT_0();

BEER_BC_LABEL(INSTR_JMP_FALSE):
	DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

	if(!frame->stackTop<Boolean>()->getData())
	{
		BEER_BC_JUMP();
	}
	else
	{
		BEER_BC_MOVE(sizeof(BEER_BC_WORD));
	}

	frame->stackPop();
	BEER_BC_NEXT_0();

BEER_BC_LABEL(INSTR_POP):
	frame->stackPop();
	BEER_BC_NEXT_0();

BEER_BC_LABEL(INSTR_TOP):
	frame->stackPush(frame->stackTop(BEER_BC_DATA(BEER_BC_WORD)));
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_WORD));

BEER_BC_LABEL(INSTR_STORE):
	frame->stackStore(BEER_BC_DATA(BEER_BC_WORD), frame->stackTop());
	frame->stackPop();
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_WORD));

BEER_BC_LABEL(INSTR_MOVE_TOP):
	{
		// if/else is an optimization
		BEER_BC_WORD move = BEER_BC_DATA(BEER_BC_WORD);
		if(move == 1)
		{
			frame->stackPush();
		}
		else if(move == 2)
		{
			frame->stackPush();
			frame->stackPush();
		}
		else
		{
			frame->stackMoveTop(move);
		}
	}
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_WORD));

BEER_BC_LABEL(INSTR_PUSH_INT64):
BEER_BC_LABEL(INSTR_PUSH_FLOAT):
BEER_BC_LABEL(INSTR_PUSH_STRING):
BEER_BC_LABEL(INSTR_PUSH_CHAR):
BEER_BC_LABEL(INSTR_PUSH_BOOL):
	frame->stackPush(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
	BEER_BC_NEXT(BEER_BC_SIZEOF(int32));

BEER_BC_LABEL(INSTR_PUSH_THIS):
	frame->stackPush(frame->stackTop(Frame::INDEX_RECEIVER));
	BEER_BC_NEXT_0();

BEER_BC_LABEL(INSTR_NEW):
	BEER_BC_CHECK_SAFEPOINT();

	frame->stackPush(); // push ret
	frame->stackPush(static_cast<Class*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)))); // push receiver
	BEER_BC_MOVE(sizeof(int32));
	frame->stackPush(static_cast<Method*>(reinterpret_cast<Object*>(BEER_BC_DATA(int32)))); // push method
	thread->openFrame();

	BEER_BC_MOVE(sizeof(int32));
	BEER_BC_RETURN();

BEER_BC_LABEL(INSTR_LOAD):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		thread->getGC()->getChild(object, object, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD));
	}
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_UWORD));

BEER_BC_LABEL(INSTR_LOAD_THIS):
	{
		StackRef<Object> object(frame, Frame::INDEX_RECEIVER);
		StackRef<Object> child(frame, frame->stackPush());
		thread->getGC()->getChild(object, child, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD));
	}
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_UWORD));

BEER_BC_LABEL(INSTR_ASSIGN):
	{

		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		StackRef<Object> child(frame, frame->stackTopIndex() - 1);
		Object::setChild(thread, object, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD), child);
		//thread->getGC()->setChild(object, child, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD)); // TODO
			
		frame->stackPop(); // pop object
		frame->stackPop(); // pop value
	}
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_UWORD));

BEER_BC_LABEL(INSTR_ASSIGN_THIS):
	{
		StackRef<Object> object(frame, Frame::INDEX_RECEIVER);
		StackRef<Object> child(frame, frame->stackTopIndex());
		Object::setChild(thread, object, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD), child);
		//thread->getGC()->setChild(object, child, Object::OBJECT_CHILDREN_COUNT + BEER_BC_DATA(BEER_BC_UWORD)); // TODO

		frame->stackPop(); // pop value
		// *NO* pop of this !!!
	}
	BEER_BC_NEXT(BEER_BC_SIZEOF(BEER_BC_UWORD));

BEER_BC_LABEL(INSTR_VIRTUAL_INVOKE):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		Method* method = NULL;

		// fetch method
		{
			uint32 index = BEER_BC_DATA(uint32);
			Class* klass = thread->getType(object);

			method = klass->getMethod(index);

			if(!method)
			{
				BEER_DBG_BREAKPOINT();
				throw MethodNotFoundException(*object, klass, NULL); // lookup failed, TODO: better exception
			}
		}

		// set new opcode
		if(false)
		{
			// set opcode

	#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH
			*reinterpret_cast<BEER_BC_OPCODE_TYPE*>(ip - sizeof(BEER_BC_OPCODE_TYPE)) = BEER_OPTIMAL_CACHED_INVOKE;
	#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
			*reinterpret_cast<BEER_BC_OPCODE_TYPE*>(ip - sizeof(BEER_BC_OPCODE_TYPE)) = Bytecode::LabelTable[BEER_OPTIMAL_CACHED_INVOKE];
	#endif // BEER_BC_DISPATCH

			// set arg
			*reinterpret_cast<int32*>(ip) = reinterpret_cast<int32>(static_cast<Object*>(method));
		}
		
		frame->stackPush(method);
		thread->openFrame();
	}
	BEER_BC_MOVE(sizeof(int32));
	BEER_BC_RETURN();

	
BEER_BC_LABEL(INSTR_SPECIAL_INVOKE):
BEER_BC_LABEL(OPTIMAL_CACHED_INVOKE):
	frame->stackPush(reinterpret_cast<Object*>(BEER_BC_DATA(int32)));
	DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Method is null"));
	thread->openFrame();
	BEER_BC_MOVE(sizeof(int32));
	BEER_BC_RETURN();
		
// no caching
BEER_BC_LABEL(INSTR_INTERFACE_INVOKE):
	{
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		NULL_ASSERT(receiver.get());

		StackRef<Method> method(frame, frame->stackPush());

		// fetch method
		{
			StackRef<String> selector(frame, frame->stackPush());
			invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), selector);

			BEER_BC_MOVE(sizeof(uint16)); // pass selector

			StackRef<PolymorphicCache> cache(frame, frame->stackPush());
			invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), cache);

			StackRef<Class> klass(frame, frame->stackPush());
			thread->getType(receiver, klass);

			PolymorphicCache::find(thread, cache, klass, selector, method);

			// lookup failed
			if(method.isNull())
			{
				throw MethodNotFoundException(*receiver, *klass, *selector);
			}

			frame->stackMoveTop(-3); // pop selector, cache, klass
		}

		thread->openFrame();
	}
	BEER_BC_MOVE(sizeof(uint16)); // pass cache
	BEER_BC_RETURN();

BEER_BC_LABEL(INSTR_STACK_INVOKE):
	thread->openFrame();
	BEER_BC_RETURN();

BEER_BC_LABEL(INSTR_RETURN):
	thread->closeFrame();
	BEER_BC_RETURN();

// optimalizations
#ifdef BEER_INLINE_OPTIMALIZATION
BEER_BC_LABEL(INLINE_BOOLEAN_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() == frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NOT_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() != frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_OR):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() || frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_AND):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() && frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NEGATION):
	frame->stackStore(frame->stackTopIndex() - 1, Boolean::makeInlineValue(
		!frame->stackTop<Boolean>(frame->stackTopIndex())->getData()
	));
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_PLUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() + frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();

		frame->stackPop(); // pop argument
		frame->stackPop(); // pop receiver

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MINUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() - frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();
		
		frame->stackPop(); // pop argument
		frame->stackPop(); // pop receiver

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MUL):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() * frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();
		
		frame->stackPop(); // pop argument
		frame->stackPop(); // pop receiver

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() == frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_NOT_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() != frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() < frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() <= frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() > frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() >= frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackPop(); // pop argument
	frame->stackPop(); // pop receiver
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_GET_LENGTH):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 1);
		Integer::IntegerData result = frame->stackTop<Array>()->getSize();
		frame->stackPop();
		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_GET_ITEM):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Array> receiver(frame, frame->stackTopIndex());

		Integer::IntegerData itemIndex = index->getData();
		BOUNDS_ASSERT(itemIndex, receiver->getSize());

		Object::getChild(thread, receiver, Array::OBJECT_CHILDREN_COUNT + itemIndex, ret);

		frame->stackPop(); // pop receiver
		frame->stackPop(); // pop index
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_SET_ITEM):
	{
		StackRef<Integer> value(frame, frame->stackTopIndex() - 2);
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Array> receiver(frame, frame->stackTopIndex());

		Integer::IntegerData itemIndex = index->getData();
		BOUNDS_ASSERT(itemIndex, receiver->getSize());

		Object::setChild(thread, receiver, Array::OBJECT_CHILDREN_COUNT + itemIndex, value);

		frame->stackMoveTop(-3); // pop index, receiver, value
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(OPTIMAL_ARRAY_ALLOC):
	{
		StackRef<Integer> length(frame, frame->stackTopIndex());
		StackRef<Array> instance(frame, frame->stackPush());

		thread->createArray(length, instance);
		//thread->getVM()->startSafePoint();
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(OPTIMAL_PUSH_TRUE):
	frame->stackPush(Boolean::True);
	BEER_BC_NEXT_0();

BEER_BC_LABEL(OPTIMAL_PUSH_FALSE):
	frame->stackPush(Boolean::False);
	BEER_BC_NEXT_0();

#endif // BEER_INLINE_OPTIMALIZATION

BEER_BC_DEFAULT_LABEL():
	throw BytecodeException(BEER_WIDEN("Unknown opcode"));
	BEER_BC_NEXT_0();

#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
LABEL_FILL_OPCODE_TABLE:
	// TODO: fill all witl LABEL_UNKNOWN_OPCODE
	// internal
	BEER_CALLABLE_LABEL(INSTR_NOP);
	BEER_CALLABLE_LABEL(INSTR_JMP);
	BEER_CALLABLE_LABEL(INSTR_JMP_TRUE);
	BEER_CALLABLE_LABEL(INSTR_JMP_FALSE);
	//BEER_CALLABLE_LABEL(INSTR_THROW);
	//BEER_CALLABLE_LABEL(INSTR_TRY_BEGIN);
	//BEER_CALLABLE_LABEL(INSTR_TRY_END);
	// stack control
	BEER_CALLABLE_LABEL(INSTR_POP);
	BEER_CALLABLE_LABEL(INSTR_TOP);
	BEER_CALLABLE_LABEL(INSTR_STORE);
	BEER_CALLABLE_LABEL(INSTR_MOVE_TOP);
	//built-in types control
	BEER_CALLABLE_LABEL(INSTR_PUSH_INT64);
	BEER_CALLABLE_LABEL(INSTR_PUSH_FLOAT);
	BEER_CALLABLE_LABEL(INSTR_PUSH_STRING);
	BEER_CALLABLE_LABEL(INSTR_PUSH_CHAR);
	BEER_CALLABLE_LABEL(INSTR_PUSH_BOOL);
	BEER_CALLABLE_LABEL(INSTR_PUSH_THIS);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_STRUCT);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_FUNC);
	// object control
	BEER_CALLABLE_LABEL(INSTR_NEW);
	BEER_CALLABLE_LABEL(INSTR_LOAD);
	BEER_CALLABLE_LABEL(INSTR_LOAD_THIS);
	BEER_CALLABLE_LABEL(INSTR_ASSIGN);
	BEER_CALLABLE_LABEL(INSTR_ASSIGN_THIS);
	// method control
	BEER_CALLABLE_LABEL(INSTR_VIRTUAL_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_INTERFACE_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_SPECIAL_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_STACK_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_RETURN);
	// Boolean
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_NOT_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_OR);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_AND);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_NEGATION);
	// Integer
	BEER_CALLABLE_LABEL(INLINE_INTEGER_PLUS);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_MINUS);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_MUL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_NOT_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_SMALLER);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_SMALLER_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_GREATER);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_GREATER_EQUAL);
	// Array
	BEER_CALLABLE_LABEL(INLINE_ARRAY_GET_LENGTH);
	BEER_CALLABLE_LABEL(INLINE_ARRAY_GET_ITEM);
	BEER_CALLABLE_LABEL(INLINE_ARRAY_SET_ITEM);
	// optimalised instructions
	BEER_CALLABLE_LABEL(OPTIMAL_ARRAY_ALLOC);
	BEER_CALLABLE_LABEL(OPTIMAL_CACHED_INVOKE);
	BEER_CALLABLE_LABEL(OPTIMAL_PUSH_TRUE);
	BEER_CALLABLE_LABEL(OPTIMAL_PUSH_FALSE);
	// important
	BEER_CALLABLE_LABEL(FILL_OPCODE_TABLE);


#ifdef BEER_BC_DEBUGGING
	for(int i = 0; i < BEER_MAX_OPCODE; i++)
	{
		LabelTableDebugDictionary[LabelTable[i]] = i;
	}
#endif // BEER_BC_DEBUGGING

#endif //BEER_BC_DISPATCH

	BEER_BC_END
}

void Bytecode::init(Thread* thread)
{
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	#define DATA_SIZE 1
	BEER_BC_UWORD data[DATA_SIZE];
	data[0] = BEER_FILL_OPCODE_TABLE;
	Bytecode bc(reinterpret_cast<byte*>(data), 1);
	
	Method m;
	m.setBytecode(&bc);
	m.setMaxStack(5);
	m.setParamsCount(0);
	m.setReturnsCount(0);
	
	Frame* frame = thread->getFrame();
	frame->stackPush();// receiver
	frame->stackPush(&m);// method
	thread->openFrame();
	Bytecode::invokeBytecode(thread);
	thread->closeFrame(); // pops method & receiver
#endif //BEER_BC_DISPATCH
}


#include "BytecodeBuilder.h"

void Bytecode::buildInvokeBytecode(Thread* thread)
{
	{
		Frame* frame = thread->getFrame();
		BEER_STACK_CHECK();

		Method* invokedMethod = frame->stackTop<Method>(-1);
		DBG_ASSERT(invokedMethod != NULL, BEER_WIDEN("Method is NULL"));

		// TODO: lock method
	
		thread->getVM()->getBytecodeBuilder()->buildBytecodeMethod(thread, invokedMethod);

		// TODO: unlock method
	}

	invokeBytecode(thread);
}