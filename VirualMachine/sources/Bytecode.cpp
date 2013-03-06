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
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeBuilder.h"
#include "MonomorphicInlineCache.h"
#include "PolymorphicInlineCache.h"
#include "Debugger.h"
#include "Array.h"

using namespace Beer;

#define BEER_BC_DISPATCH_SWITCH 0
#define BEER_BC_DISPATCH_DIRECT 1

#define BEER_BC_DISPATCH	BEER_BC_DISPATCH_SWITCH

#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH
#define BEER_BC_SAVE_OPCODE(opcode) builder.add((uint8)opcode)
#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
#define BEER_BC_SAVE_OPCODE(opcode) builder.add((void*)LabelTable[opcode])
#endif // BEER_BC_DISPATCH

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

	case BEER_INSTR_CLONE:
		cout << "CLONE";
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_INVOKE:
		cout << "INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_INTERFACEINVOKE:
		cout << "INTERFACE_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_STATIC_INVOKE:
		cout << "STATIC_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_SPECIALINVOKE:
		cout << "SPECIALINVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
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

void Bytecode::Instruction::printTranslated(VirtualMachine* vm) const
{
	switch (opcode)
	{
	case BEER_INSTR_NOP:
		cout << "NOP";
		break;

	case BEER_INSTR_JMP:
		cout << "JMP " << getData<uint16>();
		break;

	case BEER_INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData<uint16>();
		break;

	case BEER_INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData<uint16>();
		break;

	case BEER_INSTR_POP:
		cout << "POP";
		break;

	case BEER_INSTR_TOP:
		cout << "TOP " << getData<int16>();
		break;

	case BEER_INSTR_STORE:
		cout << "STORE " << getData<int16>();
		break;

	case BEER_INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData<int16>();
		break;

	case BEER_INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData<int8>());
		break;

	case BEER_INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData<int32>();
		break;

	case BEER_INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData<int64>();
		break;

	case BEER_INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData<float64>();
		break;

	case BEER_INSTR_PUSH_STRING:
		{
			Reference<String> str(vm->getHeap(), getData<uint32>());
			cout << "PUSH_STRING \"" << str->c_str() << "\"";
		}
		break;

	case BEER_INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16(); // TODO
		cout << "PUSH_CHAR " << getData<uint8>(); // TODO
		break;

	case BEER_INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData<uint8>());
		break;

	case BEER_INSTR_NEW:
		cout << "NEW " << static_cast<Class*>(reinterpret_cast<Object*>(getData<uint32>()))->getName()->c_str();
		break;

	case BEER_INSTR_CLONE:
		cout << "CLONE";
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		break;

	case BEER_INSTR_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case BEER_INSTR_INTERFACEINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "INTERFACE_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case BEER_INSTR_STATIC_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "STATIC_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case BEER_INSTR_SPECIALINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "SPECIAL_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case BEER_INSTR_RETURN:
		cout << "RETURN " << getData<uint16>();
		break;
		
	case BEER_INLINE_BOOLEAN_EQUAL:
		cout << "INLINE_BOOLEAN_EQUAL";
		break;

	case BEER_INLINE_BOOLEAN_NOT_EQUAL:
		cout << "INLINE_BOOLEAN_NOT_EQUAL";
		break;

	case BEER_INLINE_BOOLEAN_OR:
		cout << "INLINE_BOOLEAN_OR";
		break;

	case BEER_INLINE_BOOLEAN_AND:
		cout << "INLINE_BOOLEAN_AND";
		break;

	case BEER_INLINE_BOOLEAN_NEGATION:
		cout << "INLINE_BOOLEAN_NEGATION";
		break;

	case BEER_INLINE_INTEGER_PLUS:
		cout << "INLINE_INTEGER_PLUS";
		break;

	case BEER_INLINE_INTEGER_MINUS:
		cout << "INLINE_INTEGER_MINUS";
		break;

	case BEER_INLINE_INTEGER_MUL:
		cout << "INLINE_INTEGER_MUL";
		break;

	case BEER_INLINE_INTEGER_EQUAL:
		cout << "INLINE_INTEGER_EQUAL";
		break;

	case BEER_INLINE_INTEGER_NOT_EQUAL:
		cout << "INLINE_INTEGER_NOT_EQUAL";
		break;

	case BEER_INLINE_INTEGER_SMALLER:
		cout << "INLINE_INTEGER_SMALLER";
		break;

	case BEER_INLINE_INTEGER_SMALLER_EQUAL:
		cout << "INLINE_INTEGER_SMALLER_EQUAL";
		break;

	case BEER_INLINE_INTEGER_GREATER:
		cout << "INLINE_INTEGER_GREATER";
		break;

	case BEER_INLINE_INTEGER_GREATER_EQUAL:
		cout << "INLINE_INTEGER_GREATER_EQUAL";
		break;

	case BEER_INLINE_ARRAY_GET_LENGTH:
		cout << "INLINE_ARRAY_GET_LENGTH";
		break;

	case BEER_INLINE_ARRAY_GET_ITEM:
		cout << "INLINE_ARRAY_GET_ITEM";
		break;

	case BEER_INLINE_ARRAY_SET_ITEM:
		cout << "INLINE_ARRAY_SET_ITEM";
		break;

	case BEER_OPTIMAL_INTEGER_PUSH_INLINED:
		cout << "OPTIMAL_INTEGER_PUSH_INLINED" << " " << ((Integer*)(getData<Object*>()))->getData();
		break;

	case BEER_OPTIMAL_ARRAY_ALLOC:
		cout << "OPTIMAL_ARRAY_ALLOC";
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}
}

void Bytecode::build(VirtualMachine* vm, ClassFileDescriptor* classFile)
{
	BytecodeBuilder builder(mData, mDataSize, mDictSize);
	
	//int32 i = 0;

	while(!builder.done())
	{
		Bytecode::OpCode opcode = builder.next();
		//cout << i++ << "\t" << opcode << "\t"; builder.getOldInstruction()->printRaw(classFile); cout << "\n";

		switch(opcode)
		{
			// 0 bytes = 0bit

			case BEER_INSTR_NOP:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_NOP);
				break;

			case BEER_INSTR_POP:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_POP);
				break;

			case BEER_INSTR_CLONE:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_CLONE);
				break;

			case BEER_FILL_OPCODE_TABLE:
				BEER_BC_SAVE_OPCODE(BEER_FILL_OPCODE_TABLE);
				break;

			// 1 byte = 8bit

			case BEER_INSTR_PUSH_INT8:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT8);
				builder.copy(sizeof(uint8));
				break;

			case BEER_INSTR_PUSH_BOOL:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_BOOL);
				builder.copy(sizeof(uint8));
				break;

			case BEER_INSTR_PUSH_CHAR:// TODO: 16bit
				BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_CHAR);
				builder.copy(sizeof(uint8));
				break;

			// 2 bytes = 16bit

			case BEER_INSTR_RETURN:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_RETURN);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_JMP:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_JMP);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_JMP_TRUE:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_JMP_TRUE);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_JMP_FALSE:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_JMP_FALSE);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_TOP:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_TOP);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_STORE:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_STORE);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_ASSIGN:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_ASSIGN);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_LOAD:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_LOAD);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_LOAD_THIS:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_LOAD_THIS);
				builder.copy(sizeof(uint16));
				break;

			case BEER_INSTR_MOVE_TOP:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_MOVE_TOP);
				builder.copy(sizeof(uint16));
				break;

			// 4 bytes = 32bit

			case BEER_INSTR_PUSH_INT32:
				{
#ifdef BEER_INLINE_OPTIMALIZATION
					Integer::IntegerData value = builder.getData<int32>();
					if(Integer::canBeInlineValue(value))
					{
						BEER_BC_SAVE_OPCODE(BEER_OPTIMAL_INTEGER_PUSH_INLINED);
						builder.add((Object*)Integer::makeInlineValue(value));
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT32);
						builder.add(static_cast<int32>(value));
					}
				}
				break;

			case BEER_INSTR_PUSH_STRING:
				{
					BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_STRING);
					// TODO!!!
					const char16* cstring = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> string = String::gTranslate(vm, cstring);
					builder.add((int32)string.getId());
				}
				break;

			case BEER_INSTR_NEW:
				{
					const char16* cstring = classFile->getClassName(builder.getData<int32>())->c_str();
#ifdef BEER_INLINE_OPTIMALIZATION
					if(strcmp(cstring, BEER_WIDEN("Array")) == 0)
					{
						BEER_BC_SAVE_OPCODE(BEER_OPTIMAL_ARRAY_ALLOC);
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						BEER_BC_SAVE_OPCODE(BEER_INSTR_NEW);
						Reference<String> name = String::gTranslate(vm, cstring);
						Class* klass = vm->getClass(name);
						builder.add((Object*)klass); // TODO: pass Reference
					}
				}
				break;

			case BEER_INSTR_INVOKE:
			case BEER_INSTR_STATIC_INVOKE:
			case BEER_INSTR_SPECIALINVOKE:
				{
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> selector = String::gTranslate(vm, cselector);
#ifdef BEER_INLINE_OPTIMALIZATION
					Bytecode::OpCode newOpcode = vm->getInlineFunctionTable()->find(selector.get());
					if(newOpcode != BEER_INSTR_NOP)
					{
						BEER_BC_SAVE_OPCODE(newOpcode);
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						BEER_BC_SAVE_OPCODE(opcode);
						builder.add((int32)selector.getId());

						// TODO!!!
						MonomorphicInlineCache* cache = MonomorphicInlineCache::from(builder.alloc(MonomorphicInlineCache::countSize()));
						cache->clear();
					}
				}
				break;
				
			// no caching
			case BEER_INSTR_INTERFACEINVOKE:
				{
					BEER_BC_SAVE_OPCODE(BEER_INSTR_INTERFACEINVOKE);

					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> selector = String::gTranslate(vm, cselector);
					builder.add((int32)selector.getId());
					
					PolymorphicInlineCache* cache = PolymorphicInlineCache::from(builder.alloc(PolymorphicInlineCache::countSize(mMethodCachesLength)));
					cache->clear(mMethodCachesLength);
				}
				break;

			// 8 bytes = 64bit
			case BEER_INSTR_PUSH_INT64:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT64);
				builder.copy(sizeof(uint64));
				break;

			case BEER_INSTR_PUSH_FLOAT:
				BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_FLOAT);
				builder.copy(sizeof(uint64));
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}

	builder.finish(&mData, mDataSize, &mDict, mDictSize);
}

void* Bytecode::LabelTable[BEER_MAX_OPCODE * sizeof(void*)] = {0};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH

#define BEER_BC_PRINT() cout << *(uint8*)ip << "\t"; reinterpret_cast<Instruction*>(ip)->printTranslated(thread->getVM()); cout<<"\n";

#define BEER_BC_OPCODE() *(reinterpret_cast<uint8*>(ip)++)

#define BEER_BC_DATA_PTR(type) reinterpret_cast<type>(*reinterpret_cast<int32*>(ip))

#define BEER_BC_DATA(type) *reinterpret_cast<type*>(ip)

#define BEER_BC_MOVE(howmuch)

#define BEER_BC_JUMP(instri) frame->vPC = instri;

#define BEER_BC_NEXT(skip) BEER_BC_PASS();

#define BEER_BC_FETCH() ip = reinterpret_cast<byte*>(getInstruction(frame->vPC++));

#define BEER_BC_PASS() break;

#define BEER_GC_RETURN(method) return method;

#define BEER_BC_START bool cont = true; while(cont) { BEER_BC_FETCH(); switch(BEER_BC_OPCODE()) {

#define BEER_BC_END }} return NULL;

#define BEER_BC_LABEL(name) case BEER_##name

#define BEER_BC_DEFAULT_LABEL() default

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT

#define BEER_BC_PRINT() cout << *ip << "\t"; reinterpret_cast<Instruction*>(ip)->printTranslated(thread->getVM()); cout<<"\n";

#define BEER_BC_OPCODE() *(ip++)

#define BEER_BC_DATA_PTR(type) reinterpret_cast<type>(*reinterpret_cast<int32*>(ip))

#define BEER_BC_DATA(type) *reinterpret_cast<type*>(ip)

#define BEER_BC_MOVE(howmuch) ip += howmuch

#define BEER_BC_JUMP(instri) ip = reinterpret_cast<byte*>(getInstruction(frame->vPC = instri));

#define BEER_BC_NEXT(skip) BEER_BC_MOVE(skip); BEER_BC_PASS();

#define BEER_BC_FETCH() jumpAddr = *(reinterpret_cast<void**>(ip)); BEER_BC_MOVE(sizeof(void*));

#define BEER_BC_PASS() frame->vPC++; BEER_BC_FETCH(); __asm jmp jumpAddr;

#define BEER_GC_RETURN(method) return method;

#define BEER_BC_START ip = reinterpret_cast<byte*>(getInstruction(frame->vPC)); BEER_BC_PASS();

#define BEER_BC_END return NULL;

#define BEER_STORE_ADDRESS(index, label)											\
{																					\
	void** data = LabelTable;														\
	__asm lea eax, label															\
	__asm mov edx, data																\
	__asm mov [edx][index * TYPE data], eax											\
}																					\

// just a shortcut for BEER_STORE_ADDRESS with one argument
#define BEER_CALLABLE_LABEL(name) BEER_STORE_ADDRESS(BEER_##name, LABEL_##name)

#define BEER_BC_LABEL(name) LABEL_##name

// TODO
#define BEER_BC_DEFAULT_LABEL() BEER_BC_LABEL(UNKNOWN_OPCODE)

#endif // BEER_BC_DISPATCH
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Method* Bytecode::call(Thread* thread)
{
	StackFrame* frame = thread->getStackFrame();
	void* jumpAddr = NULL;
	byte* ip = NULL;

#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	if(LabelTable[BEER_FILL_OPCODE_TABLE] == NULL)
	{
		goto LABEL_FILL_OPCODE_TABLE;
	}
#endif // BEER_BC_DISPATCH

	// important for debugging
#ifdef BEER_DEBUG_MODE
	if(thread->getVM()->getDebugger()->isEnabled()) thread->getVM()->getDebugger()->step(frame);
#endif // BEER_DEBUG_MODE
		
	//DBG_ASSERT(frame->programCounter < mDictSize, BEER_WIDEN("Program counter out of range"));
	DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stack->size() - 1), BEER_WIDEN("Broken stack"));

	BEER_BC_START

BEER_BC_LABEL(INSTR_NOP):
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_JMP):
	BEER_BC_JUMP(BEER_BC_DATA(uint16));
	BEER_BC_NEXT(0); // zero is important!!!

BEER_BC_LABEL(INSTR_JMP_TRUE):
	{
		DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

		if(((Boolean*)frame->stackTop())->getData())
		{
			frame->stackPop();
			BEER_BC_JUMP(BEER_BC_DATA(uint16));
			BEER_BC_NEXT(0); // zero is important!!!
		}

		frame->stackPop();
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_JMP_FALSE):
	{
		DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

		if(!((Boolean*)frame->stackTop())->getData())
		{
			frame->stackPop();
			BEER_BC_JUMP(BEER_BC_DATA(uint16));
			BEER_BC_NEXT(0); // zero is important!!!
		}

		frame->stackPop();
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_POP):
	frame->stackPop();
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_TOP):
	frame->stackPush(frame->stackTop(BEER_BC_DATA(int16)));
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_STORE):
	frame->stackStore(BEER_BC_DATA(int16), frame->stackTop());
	frame->stackPop();
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_MOVE_TOP):
	frame->stackMoveTop(BEER_BC_DATA(int16));
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_PUSH_INT8):
	{
		StackRef<Integer> ret(frame, frame->stackPush());
		thread->createInteger(ret, BEER_BC_DATA(int8));
	}
	BEER_BC_NEXT(sizeof(int8));
		
BEER_BC_LABEL(INSTR_PUSH_INT32):
	{
		StackRef<Integer> ret(frame, frame->stackPush());
		thread->createInteger(ret, BEER_BC_DATA(int32));
	}
	BEER_BC_NEXT(sizeof(int32));

BEER_BC_LABEL(INSTR_PUSH_INT64):
	{
		StackRef<Integer> ret(frame, frame->stackPush());
		thread->createInteger(ret, static_cast<Integer::IntegerData>(BEER_BC_DATA(int64)));
	}
	BEER_BC_NEXT(sizeof(int64));

BEER_BC_LABEL(INSTR_PUSH_FLOAT):
	{
		StackRef<Float> ret(frame, frame->stackPush());
		thread->createFloat(ret, static_cast<Float::FloatData>(BEER_BC_DATA(float64)));
	}
	BEER_BC_NEXT(sizeof(float64));

BEER_BC_LABEL(INSTR_PUSH_STRING):
	frame->stackPush(thread->getHeap()->translate(BEER_BC_DATA(int32)));
	BEER_BC_NEXT(sizeof(int32));

BEER_BC_LABEL(INSTR_PUSH_CHAR):
	frame->stackPush(Character::makeInlineValue(BEER_BC_DATA(uint8))); // TODO: char16
	BEER_BC_NEXT(sizeof(uint8));

BEER_BC_LABEL(INSTR_PUSH_BOOL):
	frame->stackPush(Boolean::makeInlineValue(BEER_BC_DATA(int8) == 1));
	BEER_BC_NEXT(sizeof(int8));

BEER_BC_LABEL(INSTR_NEW):
	{
		StackRef<Object> instance(frame, frame->stackPush());
		StackRef<Class> klass(frame, frame->stackPush(BEER_BC_DATA_PTR(Object*))); // TODO
		thread->createInstance(klass, instance);
	}
	BEER_BC_NEXT(sizeof(Object*));

BEER_BC_LABEL(INSTR_CLONE):
	throw Exception(BEER_WIDEN("Not yet implemented"), __WFILE__, __LINE__);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_ASSIGN):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		object->setChild(2 + BEER_BC_DATA(int16), frame->stackTop(frame->stackTopIndex() - 1)); // TODO: find start of properties
			
		frame->stackMoveTop(-2);
	}
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_LOAD):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		// pops object and push a child
		object = object->getChild(2 + BEER_BC_DATA(uint16)); // TODO: find start of properties
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_LOAD_THIS):
	{
		StackRef<Object> object(frame, 0);
		NULL_ASSERT(object.get());

		frame->stackPush(object->getChild(2 + BEER_BC_DATA(uint16))); // TODO: find start of properties

		// *NO* pop of this !!!
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_INVOKE):
BEER_BC_LABEL(INSTR_STATIC_INVOKE):
BEER_BC_LABEL(INSTR_SPECIALINVOKE):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		Reference<String> selector(thread->getHeap(), BEER_BC_DATA(int32));

		// find method using inline cache
		MonomorphicInlineCache* cache = MonomorphicInlineCache::from(ip + sizeof(int32));
		Class* klass = thread->getVM()->getClass(object);
		Method* method = cache->find(klass, *selector);

		// lookup failed
		if(!method)
		{
			cache->find(klass, *selector);
			throw MethodNotFoundException(*object, klass, *selector);
		}
				
		// invoke method
		//frame->programCounter = nextInstruction;
		BEER_BC_MOVE(sizeof(int32) + MonomorphicInlineCache::countSize());
		BEER_GC_RETURN(method);
	}
	BEER_BC_NEXT(0); // should not happen
		
// no caching
BEER_BC_LABEL(INSTR_INTERFACEINVOKE):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		Reference<String> selector(thread->getHeap(), BEER_BC_DATA(int32));

		// find method using inline cache
		PolymorphicInlineCache* cache = PolymorphicInlineCache::from(ip + sizeof(int32));
		Class* klass = thread->getVM()->getClass(object); // TODO
		Method* method = cache->find(klass, selector.get(), mMethodCachesLength);

		// lookup failed
		if(!method)
		{
			throw MethodNotFoundException(*object, thread->getVM()->getClass(object), *selector);
		}

		// invoke method
		//frame->programCounter = nextInstruction;
		BEER_BC_MOVE(sizeof(int32) + PolymorphicInlineCache::countSize(mMethodCachesLength));
		BEER_GC_RETURN(method);
	}
	BEER_BC_NEXT(0); // should not happen

BEER_BC_LABEL(INSTR_RETURN):
	frame->stackMoveTop(-static_cast<int32>(BEER_BC_DATA(uint16)));
	BEER_BC_MOVE(sizeof(uint16));
	BEER_GC_RETURN(NULL);

// optimalizations
#ifdef BEER_INLINE_OPTIMALIZATION
BEER_BC_LABEL(INLINE_BOOLEAN_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Boolean*>((int32)0)->getData() == frame->stack->top<Boolean*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NOT_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Boolean*>((int32)0)->getData() != frame->stack->top<Boolean*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_OR):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Boolean*>((int32)0)->getData() || frame->stack->top<Boolean*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_AND):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Boolean*>((int32)0)->getData() && frame->stack->top<Boolean*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NEGATION):
	frame->stack->set(Boolean::makeInlineValue(
		!frame->stack->top<Boolean*>((int32)0)->getData()
	), -1);
	frame->stackMoveTop(-1);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_PLUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stack->top<Integer*>(0)->getData() + frame->stack->top<Integer*>(-1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MINUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stack->top<Integer*>(0)->getData() - frame->stack->top<Integer*>(-1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MUL):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stack->top<Integer*>(0)->getData() * frame->stack->top<Integer*>(-1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() == frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_NOT_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() != frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() < frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() <= frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() > frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER_EQUAL):
	frame->stack->set(Boolean::makeInlineValue(
		frame->stack->top<Integer*>((int32)0)->getData() >= frame->stack->top<Integer*>(-1)->getData()
	), -2);
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_GET_LENGTH):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 1);
		Integer::IntegerData result = frame->stack->top<Array*>((int32)0)->getSize();
		frame->stackMoveTop(-1);
		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_GET_ITEM):
	{
		StackRef<Array> receiver(frame, frame->stackTopIndex());
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);

		BOUNDS_ASSERT(index->getData(), receiver->getSize());
		ret = static_cast<Integer*>(receiver->getItem(index->getData()));

		frame->stackMoveTop(-2);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_SET_ITEM):
	{
		StackRef<Array> receiver(frame, frame->stackTopIndex());
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Integer> value(frame, frame->stackTopIndex() - 2);

		BOUNDS_ASSERT(index->getData(), receiver->getSize());
		receiver->setItem(index->getData(), value.get());

		frame->stackMoveTop(-3);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(OPTIMAL_INTEGER_PUSH_INLINED):
	{
		frame->stackPush(static_cast<Integer*>(BEER_BC_DATA_PTR(Object*)));
	}
	BEER_BC_NEXT(sizeof(Object*));

BEER_BC_LABEL(OPTIMAL_ARRAY_ALLOC):
	{
		StackRef<Integer> length(frame, frame->stackTopIndex());
		StackRef<Array> instance(frame, frame->stackPush());

		StackRef<Integer> copiedLength(frame, frame->stackPush(*length)); // TODO: better syntax
		thread->createArray(copiedLength, instance); // pops copied length
	}
	BEER_BC_NEXT(0);
#endif // BEER_INLINE_OPTIMALIZATION

BEER_BC_DEFAULT_LABEL():
	throw BytecodeException(BEER_WIDEN("Unknown opcode"));
	BEER_BC_NEXT(0);

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
	BEER_CALLABLE_LABEL(INSTR_PUSH_INT8);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_INT16);
	BEER_CALLABLE_LABEL(INSTR_PUSH_INT32);
	BEER_CALLABLE_LABEL(INSTR_PUSH_INT64);
	BEER_CALLABLE_LABEL(INSTR_PUSH_FLOAT);
	BEER_CALLABLE_LABEL(INSTR_PUSH_STRING);
	BEER_CALLABLE_LABEL(INSTR_PUSH_CHAR);
	BEER_CALLABLE_LABEL(INSTR_PUSH_BOOL);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_STRUCT);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_FUNC);
	// object control
	BEER_CALLABLE_LABEL(INSTR_NEW);
	//BEER_CALLABLE_LABEL(INSTR_DELETE);
	BEER_CALLABLE_LABEL(INSTR_CLONE);
	BEER_CALLABLE_LABEL(INSTR_ASSIGN);
	BEER_CALLABLE_LABEL(INSTR_LOAD);
	BEER_CALLABLE_LABEL(INSTR_LOAD_THIS);
	// method control
	BEER_CALLABLE_LABEL(INSTR_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_INTERFACEINVOKE);
	BEER_CALLABLE_LABEL(INSTR_STATIC_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_SPECIALINVOKE);
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
	BEER_CALLABLE_LABEL(OPTIMAL_INTEGER_PUSH_INLINED);
	BEER_CALLABLE_LABEL(OPTIMAL_ARRAY_ALLOC);
	// important
	BEER_CALLABLE_LABEL(FILL_OPCODE_TABLE);
#endif //BEER_BC_DISPATCH

	BEER_BC_END
}

void Bytecode::init(Thread* thread)
{
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	#define DATA_SIZE 1
	byte data[DATA_SIZE];
	data[0] = BEER_FILL_OPCODE_TABLE;
	Bytecode bc(data, 1, 1);
	StackFrame* frame = thread->openStackFrame();
	frame->stackPush();
	//bc.build((VirtualMachine*)thread, NULL); // TODO
	bc.call(thread, frame);
	frame->stackPop();
	thread->closeStackFrame();
#endif //BEER_BC_DISPATCH
}